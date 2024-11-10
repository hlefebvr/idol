//
// Created by henri on 27/01/23.
//
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"

idol::Model::Model(Env &t_env, Storage t_storage) : m_env(t_env), m_id(t_env.create_model_id()), m_storage(t_storage) {}

idol::Model::Model(idol::Model && t_src) noexcept
    : m_env(t_src.m_env),
      m_id(t_src.m_id),
      m_sense(t_src.m_sense),
      m_variables(std::move(t_src.m_variables)),
      m_constraints(std::move(t_src.m_constraints)),
      m_optimizer_factory(std::move(t_src.m_optimizer_factory))
{

    t_src.m_has_been_moved = true;

}


idol::Model::~Model() {

    if (m_has_been_moved) {
        return;
    }

    for (const auto& var : m_variables) {
        m_env.remove_version(*this, var);
    }
    m_variables.clear();

    for (const auto& ctr : m_constraints) {
        m_env.remove_version(*this, ctr);
    }
    m_constraints.clear();

    m_env.free_model_id(*this);

}


void idol::Model::add(const Var &t_var, TempVar t_temp_var) {

    throw_if_unknown_object(t_temp_var.column());

    const unsigned int index = m_variables.size();
    const double lb = t_temp_var.lb();
    const double ub = t_temp_var.ub();
    const auto type = t_temp_var.type();
    const double obj = t_temp_var.obj();

    // Create variable version
    m_env.create_version(*this,t_var,index, lb, ub, type, obj, std::move(t_temp_var.column()));
    m_variables.emplace_back(t_var);

    // Add to objective
    m_objective.linear().set(t_var, obj);

    // Update rows if necessary
    if (m_storage != ColumnOriented || m_has_minor_representation) {
        add_column_to_rows(t_var);
    }

    // If there is an optimizer, we notify it
    if (has_optimizer()) {
        optimizer().add(t_var);
    }

    // Do not track the column if not necessary
    if (m_storage == RowOriented && !m_has_minor_representation) {
        m_env.version(*this, t_var).reset_column();
    }

}

void idol::Model::add_column_to_rows(const idol::Var &t_var) {

    const auto& column = get_var_column(t_var);

    for (const auto& [ctr, constant] : column) {
        auto& version = m_env.version(*this, ctr);
        if (version.has_row()) {
            version.row().set(t_var, constant);
        }
    }

}

void idol::Model::add(const Var &t_var) {
    const auto& default_version = m_env[t_var];
    add(t_var, TempVar(
             default_version.lb(),
             default_version.ub(),
             default_version.type(),
             default_version.obj(),
             LinExpr<Ctr>(default_version.column())
    ));
}

void idol::Model::remove(const Var &t_var) {

    if (has_optimizer()) {
        optimizer().remove(t_var);
    }

    m_objective.linear().remove(t_var);

    auto& version = m_env.version(*this, t_var);

    if (!version.has_column()) {

        for (const auto& ctr : m_constraints) {
            auto& ctr_version = m_env.version(*this, ctr);
            if (ctr_version.has_row()) {
                auto& row = ctr_version.row();
                row.remove(t_var);
            }
        }

    } else {

        for (const auto& [ctr, constant] : version.column()) {
            auto& ctr_version = m_env.version(*this, ctr);
            if (ctr_version.has_row()) {
                auto& row = ctr_version.row();
                row.remove(t_var);
            }
        }

    }

    /*
    unsigned int index = version.index();
    m_variables.erase(m_variables.begin() + index);

    // Shifting all indices left after the deleted constraint
    for (const unsigned int n_vars = m_variables.size(); index < n_vars ; ++index) {
        m_env.version(*this, m_variables.at(index)).set_index(index);
    }
    m_env.remove_version(*this, t_var);
     */

    const auto index = m_env.version(*this, t_var).index();
    m_env.version(*this, m_variables.back()).set_index(index);
    m_variables[index] = m_variables.back();
    m_variables.pop_back();
    m_env.remove_version(*this, t_var);
}

void idol::Model::remove(const Ctr &t_ctr) {

    if (has_optimizer()) {
        optimizer().remove(t_ctr);
    }

    auto& version = m_env.version(*this, t_ctr);
    m_rhs.remove(t_ctr);

    if (!version.has_row()) {

        for (const auto& var : m_variables) {
            auto& var_version = m_env.version(*this, var);
            if (var_version.has_column()) {
                auto& column = var_version.column();
                column.remove(t_ctr);
            }
        }

    } else {

        for (const auto& [var, constant] : version.row()) {
            auto& var_version = m_env.version(*this, var);
            if (var_version.has_column()) {
                auto& column = var_version.column();
                column.remove(t_ctr);
            }
        }

    }

    /*
    unsigned int index = version.index();
    m_constraints.erase(m_constraints.begin() + index);

    // Shifting all indices left after the deleted constraint
    for (const unsigned int n_ctrs = m_constraints.size(); index < n_ctrs ; ++index) {
        m_env.version(*this, m_constraints.at(index)).set_index(index);
    }
    m_env.remove_version(*this, t_ctr);
     */

    const auto index = m_env.version(*this, t_ctr).index();
    m_env.version(*this, m_constraints.back()).set_index(index);
    m_constraints[index] = m_constraints.back();
    m_constraints.pop_back();
    m_env.remove_version(*this, t_ctr);
}

void idol::Model::add(const Ctr &t_ctr, TempCtr t_temp_ctr) {

    throw_if_unknown_object(t_temp_ctr.lhs());

    const unsigned int index = m_constraints.size();
    const auto type = t_temp_ctr.type();
    const double rhs = t_temp_ctr.rhs();

    // Create constraint version
    m_env.create_version(*this, t_ctr, index, std::move(t_temp_ctr.lhs()), type, rhs);
    m_constraints.emplace_back(t_ctr);

    // Update rhs
    m_rhs.set(t_ctr, rhs);

    // Update columns if necessary
    if (m_storage != RowOriented || m_has_minor_representation) {
        add_row_to_columns(t_ctr);
    }

    if (has_optimizer()) {
        optimizer().add(t_ctr);
    }

    // Do not track the row if not necessary
    if (m_storage == ColumnOriented && !m_has_minor_representation) {
        m_env.version(*this, t_ctr).reset_row();
    }

}

void idol::Model::add_row_to_columns(const idol::Ctr &t_ctr) {

    const auto& lhs = get_ctr_row(t_ctr);

    for (const auto& [var, constant] : lhs) {
        auto& version = m_env.version(*this, var);
        if (version.has_column()) {
            version.column().set(t_ctr, constant);
        }
    }

}

void idol::Model::add(const Ctr &t_ctr) {
    const auto& default_version = m_env[t_ctr];
    add(t_ctr,
            TempCtr(
                LinExpr<Var>(default_version.lhs()),
                default_version.type(),
                default_version.rhs()
            )
        );
}

idol::ObjectiveSense idol::Model::get_obj_sense() const {
    return m_sense;
}

const idol::Expr<idol::Var, idol::Var>& idol::Model::get_obj_expr() const {
    return m_objective;
}

const idol::LinExpr<idol::Ctr>& idol::Model::get_rhs_expr() const {
    return m_rhs;
}

double idol::Model::get_mat_coeff(const Ctr &t_ctr, const Var &t_var) const {
    if (m_storage == Storage::ColumnOriented) {
        return m_env.version(*this, t_var).column().get(t_ctr);
    }
    return m_env.version(*this, t_ctr).row().get(t_var);
}

idol::LinExpr<idol::Var> idol::Model::get_ctr_row(const Ctr &t_ctr) const {
    auto& version = m_env.version(*this, t_ctr);

    if (!version.has_row()) {
        const_cast<Model*>(this)->build_row(t_ctr);
        m_has_minor_representation = true;
    }

    return version.row();
}

idol::CtrType idol::Model::get_ctr_type(const Ctr &t_ctr) const {
    return m_env.version(*this, t_ctr).type();
}

unsigned int idol::Model::get_ctr_index(const Ctr &t_ctr) const {
    return (int) m_env.version(*this, t_ctr).index();
}

double idol::Model::get_var_lb(const Var &t_var) const {
    return m_env.version(*this, t_var).lb();
}

double idol::Model::get_var_ub(const Var &t_var) const {
    return m_env.version(*this, t_var).ub();
}

const idol::LinExpr<idol::Ctr> &idol::Model::get_var_column(const Var &t_var) const {
    auto& version = m_env.version(*this, t_var);

    if (!version.has_column()) {
        const_cast<Model*>(this)->build_column(t_var);
        m_has_minor_representation = true;
    }

    return version.column();
}

unsigned int idol::Model::get_var_index(const Var &t_var) const {
    return m_env.version(*this, t_var).index();
}

idol::VarType idol::Model::get_var_type(const Var &t_var) const {
    return m_env.version(*this, t_var).type();
}

bool idol::Model::has(const Var &t_var) const {
    return m_env.has_version(*this, t_var);
}

bool idol::Model::has(const Ctr &t_ctr) const {
    return m_env.has_version(*this, t_ctr);
}

idol::Model* idol::Model::clone() const {
    return new Model(*this);
}

idol::Model::Model(const Model& t_src) : Model(t_src.m_env) {

    /**
     * TODO: here, it would be more efficient to directly copy the data from the source model
     * To do this, we need to add a method to the Env
     */

    reserve_vars(t_src.vars().size());
    reserve_ctrs(t_src.ctrs().size());

    m_storage = t_src.m_storage;

    if (t_src.m_storage == Both || t_src.m_storage == RowOriented) {

        for (const auto& var : t_src.vars()) {
            add(var, TempVar(
                    t_src.get_var_lb(var),
                    t_src.get_var_ub(var),
                    t_src.get_var_type(var),
                    t_src.get_var_obj(var),
                    LinExpr<Ctr>()
            ));
        }

        for (const auto& ctr : t_src.ctrs()) {
            add(ctr, TempCtr(
                    t_src.get_ctr_row(ctr),
                    t_src.get_ctr_type(ctr),
                    t_src.get_ctr_rhs(ctr)
            ));
        }

        set_obj_sense(t_src.get_obj_sense());
        set_obj_expr(t_src.get_obj_expr());

    } else {

        for (const auto& ctr : t_src.ctrs()) {
            add(ctr, TempCtr(
                    LinExpr<Var>(),
                    t_src.get_ctr_type(ctr),
                    t_src.get_ctr_rhs(ctr)
            ));
        }

        for (const auto& var : t_src.vars()) {
            add(var, TempVar(
                    t_src.get_var_lb(var),
                    t_src.get_var_ub(var),
                    t_src.get_var_type(var),
                    t_src.get_var_obj(var),
                    LinExpr<Ctr>(t_src.get_var_column(var))
            ));
        }

        set_obj_sense(t_src.get_obj_sense());
        set_obj_expr(t_src.get_obj_expr());

    }

    if (t_src.m_optimizer_factory) {
        use(*t_src.m_optimizer_factory);
    }

}

void idol::Model::optimize() {
    optimizer().optimize();
}

void idol::Model::write(const std::string& t_name) {
    optimizer().write(t_name);
}

void idol::Model::update() {
    optimizer().update();
}

void idol::Model::use(const OptimizerFactory &t_optimizer_factory) {
    m_optimizer.reset(t_optimizer_factory(*this));
    m_optimizer->build();
    m_optimizer_factory.reset(t_optimizer_factory.clone());
}

void idol::Model::unuse() {
    m_optimizer.reset();
    m_optimizer_factory.reset();
}

bool idol::Model::has_optimizer() const {
    return bool(m_optimizer);
}

idol::SolutionStatus idol::Model::get_status() const {
    return optimizer().get_status();
}

idol::SolutionReason idol::Model::get_reason() const {
    return optimizer().get_reason();
}

double idol::Model::get_best_obj() const {
    return optimizer().get_best_obj();
}

double idol::Model::get_var_primal(const Var &t_var) const {
    return optimizer().get_var_primal(t_var);
}

double idol::Model::get_ctr_farkas(const Ctr &t_ctr) const {
    return optimizer().get_ctr_farkas(t_ctr);
}

double idol::Model::get_ctr_dual(const Ctr &t_ctr) const {
    return optimizer().get_ctr_dual(t_ctr);
}

double idol::Model::get_var_ray(const Var &t_var) const {
    return optimizer().get_var_ray(t_var);
}

double idol::Model::get_best_bound() const {
    return optimizer().get_best_bound();
}

void idol::Model::set_obj_sense(ObjectiveSense t_value) {

    if (t_value != Minimize && t_value != Maximize) {
        throw Exception("Unsupported objective sense.");
    }

    m_sense = t_value;

    if (has_optimizer()) {
        optimizer().update_obj_sense();
    }

}

void idol::Model::set_obj_expr(const Expr<Var, Var> &t_objective) {
    set_obj_expr(Expr<Var, Var>(t_objective));
}

void idol::Model::set_obj_expr(Expr<Var, Var> &&t_objective) {

    throw_if_unknown_object(t_objective.linear());

    for (const auto& [var, val] : m_objective.linear()) {
        auto& version = m_env.version(*this, var);
        version.set_obj(0.);
    }

    m_objective = std::move(t_objective);

    for (const auto& [var, val] : m_objective.linear()) {
        auto& version = m_env.version(*this, var);
        version.set_obj(val);
    }

    if (has_optimizer()) {
        optimizer().update_obj();
    }

}

void idol::Model::set_rhs_expr(const idol::LinExpr<idol::Ctr> &t_rhs) {
    set_rhs_expr(LinExpr<Ctr>(t_rhs));
}

void idol::Model::set_rhs_expr(LinExpr<Ctr> &&t_rhs) {

    throw_if_unknown_object(t_rhs);

    for (const auto& [ctr, val] : m_rhs) {
        auto& version = m_env.version(*this, ctr);
        version.set_rhs(0.);
    }

    m_rhs = std::move(t_rhs);

    for (const auto& [ctr, val] : m_rhs) {
        auto& version = m_env.version(*this, ctr);
        version.set_rhs(val);
    }

    if (has_optimizer()) {
        optimizer().update_rhs();
    }

}

void idol::Model::set_obj_const(double t_constant) {

    m_objective.constant() = t_constant;

    if (has_optimizer()) {
        optimizer().update_obj_constant();
    }

}

void idol::Model::set_mat_coeff(const Ctr &t_ctr, const Var &t_var, double t_coeff) {

    if (column_storage_matters()) {
        auto& column = m_env.version(*this, t_var).column();
        column.set(t_ctr, t_coeff);
    }

    if (row_storage_matters()) {
        auto& row = m_env.version(*this, t_ctr).row();
        row.set(t_var, t_coeff);
    }

    if (has_optimizer()) {
        optimizer().update();
        optimizer().update_mat_coeff(t_ctr, t_var);
    }

}

void idol::Model::set_ctr_rhs(const Ctr &t_ctr, double t_rhs) {

    m_env.version(*this, t_ctr).set_rhs(t_rhs);
    m_rhs.set(t_ctr, t_rhs);

    if (has_optimizer()) {
        optimizer().update_ctr_rhs(t_ctr);
    }

}

void idol::Model::set_ctr_type(const Ctr &t_ctr, CtrType t_type) {

    m_env.version(*this, t_ctr).set_type(t_type);

    if (has_optimizer()) {
        optimizer().update_ctr_type(t_ctr);
    }

}

void idol::Model::set_ctr_row(const Ctr &t_ctr, const LinExpr<Var> &t_row) {
    set_ctr_row(t_ctr, LinExpr<Var>(t_row));
}

void idol::Model::set_ctr_row(const Ctr &t_ctr, LinExpr<Var> &&t_row) {

    std::list<Var> variables_to_update;

    auto& version = m_env.version(*this, t_ctr);

    bool row_has_been_created = false;

    // First, we delete the existing row
    // 1. we create the current row if it does not exist
    if (!version.has_row()) {
        build_row(t_ctr);
        row_has_been_created = true;
    }

    // 2. we replace every entry from the old row by zero in the existing columns
    for (const auto& [var, constant] : version.row()) {
        auto& var_version = m_env.version(*this, var);
        if (var_version.has_column()) {
            auto& column = var_version.column();
            column.set(t_ctr, 0.);
        }
        variables_to_update.emplace_back(var);
    }

    // 3. if we created the row in this method call, we delete it immediately
    if (row_has_been_created) {
        version.reset_row();
    }

    // 4. we now update each column with the appropriate values
    for (const auto& [var, constant] : t_row) {
        auto& var_version = m_env.version(*this, var);
        if (var_version.has_column()) {
            auto& column = var_version.column();
            column.set(t_ctr, 0.);
        }
        variables_to_update.emplace_back(var);
    }

    // 5. finally, we set the row to the new value
    if (row_storage_matters()) {
        version.set_row(std::move(t_row));
    }

    if (has_optimizer()) {
        optimizer().update();
        for (const auto& var : variables_to_update) {
            optimizer().update_mat_coeff(t_ctr, var);
        }
    }
}

void idol::Model::set_var_type(const Var &t_var, VarType t_type) {

    m_env.version(*this, t_var).set_type(t_type);

    if (has_optimizer()) {
        optimizer().update_var_type(t_var);
    }

}

void idol::Model::set_var_lb(const Var &t_var, double t_lb) {

    m_env.version(*this, t_var).set_lb(t_lb);

    if (has_optimizer()) {
        optimizer().update_var_lb(t_var);
    }

}

void idol::Model::set_var_ub(const Var &t_var, double t_ub) {

    m_env.version(*this, t_var).set_ub(t_ub);

    if (has_optimizer()) {
        optimizer().update_var_ub(t_var);
    }

}

void idol::Model::set_var_obj(const Var &t_var, double t_obj) {

    m_env.version(*this, t_var).set_obj(t_obj);
    m_objective.linear().set(t_var, t_obj);

    if (has_optimizer()) {
        optimizer().update_var_obj(t_var);
    }

}

void idol::Model::set_var_column(const Var &t_var, const LinExpr<Ctr> &t_column) {
    set_var_column(t_var, LinExpr<Ctr>(t_column));
}

void idol::Model::set_var_column(const Var &t_var, LinExpr<Ctr> &&t_column) {

    std::list<Ctr> constraints_to_update;

    auto& version = m_env.version(*this, t_var);

    //t_column.sort_by_index();
    //t_column.reduce();

    bool column_has_been_created = false;

    // First, we delete the existing column
    // 1. we create the current column if it does not exist
    if (!version.has_column()) {
        build_column(t_var);
        column_has_been_created = true;
    }

    // 2. we replace every entry from the old column by zero in the existing rows
    for (const auto& [ctr, constant] : version.column()) {
        auto& ctr_version = m_env.version(*this, ctr);
        if (ctr_version.has_row()) {
            auto& row = ctr_version.row();
            row.set(t_var, 0.);
        }
        constraints_to_update.emplace_back(ctr);
    }

    // 3. if we created the row in this method call, we delete it immediately
    if (column_has_been_created) {
        version.reset_column();
    }

    // 4. we now update each row with the appropriate values
    for (const auto& [ctr, constant] : t_column) {
        auto& ctr_version = m_env.version(*this, ctr);
        if (ctr_version.has_row()) {
            auto& row = ctr_version.row();
            row.set(t_var, 0.);
        }
        constraints_to_update.emplace_back(ctr);
    }

    // 5. finally, we set the column to the new value
    if (row_storage_matters()) {
        version.set_column(std::move(t_column));
    }

    if (has_optimizer()) {
        optimizer().update();
        for (const auto& ctr : constraints_to_update) {
            optimizer().update_mat_coeff(ctr, t_var);
        }
    }

}

idol::Var idol::Model::add_var(double t_lb, double t_ub, VarType t_type, double t_obj, std::string t_name) {
    return add_var(t_lb, t_ub, t_type, t_obj, LinExpr<Ctr>(), std::move(t_name));
}

idol::Var idol::Model::add_var(double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr> t_column, std::string t_name) {
    Var result(m_env, t_lb, t_ub, t_type, t_obj, std::move(t_column), std::move(t_name));
    add(result);
    return result;
}

idol::Ctr idol::Model::add_ctr(LinExpr<Var> &&t_row, CtrType t_type, double t_rhs, std::string t_name) {
    return add_ctr(TempCtr(std::move(t_row), t_type, t_rhs), std::move(t_name));
}

idol::Ctr idol::Model::add_ctr(TempCtr t_temp_ctr, std::string t_name) {
    Ctr result(m_env, std::move(t_temp_ctr), std::move(t_name));
    add(result);
    return result;
}

unsigned int idol::Model::get_n_solutions() const {
    return optimizer().get_n_solutions();
}

unsigned int idol::Model::get_solution_index() const {
    return optimizer().get_solution_index();
}

void idol::Model::set_solution_index(unsigned int t_index) {
    optimizer().set_solution_index(t_index);
}

idol::Ctr idol::Model::get_ctr_by_index(unsigned int t_index) const {
    return m_constraints.at(t_index);
}

idol::Var idol::Model::get_var_by_index(unsigned int t_index) const {
    return m_variables.at(t_index);
}

idol::Model idol::Model::copy() const {
    return Model(*this); // NOLINT(*-return-braced-init-list)
}

void idol::Model::reserve_vars(unsigned int t_size) {
    m_variables.reserve(t_size);
}

void idol::Model::reserve_ctrs(unsigned int t_size) {
    m_constraints.reserve(t_size);
}

double idol::Model::get_var_reduced_cost(const idol::Var &t_var) const {
    return optimizer().get_var_reduced_cost(t_var);
}

void idol::Model::build_row(const idol::Ctr &t_ctr) {

    LinExpr<Var> row;
    for (const auto& var : m_variables) {
        const auto& column = get_var_column(var);
        const double value = column.get(t_ctr);
        row.set(var, value);
    }

    m_env.version(*this, t_ctr).set_row(std::move(row));

}

void idol::Model::build_column(const idol::Var &t_var) {

    LinExpr<Ctr> column;
    for (const auto& ctr : m_constraints) {
        const auto& lhs = get_ctr_row(ctr);
        const double value = lhs.get(t_var);
        column.set(ctr, value);
    }

    m_env.version(*this, t_var).set_column(std::move(column));

}

template<class T>
void idol::Model::throw_if_unknown_object(const idol::LinExpr<T> &t_expr) {
    for (const auto& [obj, constant] : t_expr) {
        if (!has(obj)) {
            throw Exception("Object " + obj.name() + " is not part of the model.");
        }
    }
}

void idol::Model::dump(std::ostream &t_os) const {

    t_os << "Objective Function:\n";
    t_os << "-------------------\n";

    t_os << get_obj_expr() << '\n';

    t_os << "RHS:\n";
    t_os << "----\n";

    t_os << get_rhs_expr() << '\n';
    const int cell_width = 6;  // Fixed cell width for consistent alignment

    // Column-Oriented Representation
    t_os << "Column Oriented Representation:\n";
    t_os << "-------------------------------\n";

    // Print variable names with fixed width for header alignment
    t_os << std::setw(cell_width) << " ";
    for (const auto& var : vars()) {
        t_os << std::setw(cell_width) << var;
    }
    t_os << '\n';

    // Print each row with fixed-width formatting
    for (const auto& ctr : ctrs()) {
        t_os << std::setw(cell_width) << ctr;
        for (const auto& var : vars()) {
            auto& version = m_env.version(*this, var);
            if (version.has_column()) {
                const auto& linear = version.column();
                if (linear.has_index(ctr)) {
                    t_os << std::setw(cell_width) << linear.get(ctr);
                } else {
                    t_os << std::setw(cell_width) << '_';
                }
            } else {
                t_os << std::setw(cell_width) << 'X';
            }
        }
        t_os << '\n';
    }

    // Row-Oriented Representation
    t_os << "\nRow Oriented Representation:\n";
    t_os << "----------------------------\n";

    t_os << std::setw(cell_width) << " ";
    for (const auto& var : vars()) {
        t_os << std::setw(cell_width) << var;
    }
    t_os << '\n';

    for (const auto& ctr : ctrs()) {
        t_os << std::setw(cell_width) << ctr;
        for (const auto& var : vars()) {
            auto& version = m_env.version(*this, ctr);
            if (version.has_row()) {
                const auto& linear = version.row();
                if (linear.has_index(var)) {
                    t_os << std::setw(cell_width) << linear.get(var);
                } else {
                    t_os << std::setw(cell_width) << '_';
                }
            } else {
                t_os << std::setw(cell_width) << 'X';
            }
        }
        t_os << '\n';
    }

}

void idol::Model::set_storage(idol::Model::Storage t_storage, bool t_reset_minor_representation) {

    if (t_storage == m_storage) {
        if (t_reset_minor_representation) {
            reset_minor_representation();
        }
        return;
    }

    if (m_storage == Both) {
        m_storage = t_storage;
        if (t_reset_minor_representation) {
            reset_minor_representation();
        }
        return;
    }

    if (t_storage == Both) {
        if (m_storage == ColumnOriented) {
            build_rows();
        } else {
            build_columns();
        }
    } else if (t_storage == ColumnOriented) {
        build_columns();
    } else if (t_storage == RowOriented) {
        build_rows();
    } else { [[unlikely]];
        throw Exception("Unsupported storage type.");
    }

    m_storage = t_storage;

    if (t_reset_minor_representation) {
        reset_minor_representation();
    }

}

void idol::Model::reset_minor_representation() {

    m_has_minor_representation = false;

    if (m_storage == ColumnOriented) {
        for (const auto& ctr : m_constraints) {
            m_env.version(*this, ctr).reset_row();
        }
        return;
    }

    if (m_storage == RowOriented) {
        for (const auto& var : m_variables) {
            m_env.version(*this, var).reset_column();
        }
        return;
    }

}

void idol::Model::build_rows() {
    for (const auto& ctr : m_constraints) {
        if (!m_env.version(*this, ctr).has_row()) {
            build_row(ctr);
        }
    }
}

void idol::Model::build_columns() {
    for (const auto& var : m_variables) {
        if (!m_env.version(*this, var).has_column()) {
            build_column(var);
        }
    }
}

double idol::Model::get_var_obj(const idol::Var &t_var) const {
    return m_env.version(*this, t_var).obj();
}

double idol::Model::get_ctr_rhs(const idol::Ctr &t_ctr) const {
    return m_env.version(*this, t_ctr).rhs();
}

bool idol::Model::column_storage_matters() const {
    return m_has_minor_representation || m_storage != RowOriented;
}

bool idol::Model::row_storage_matters() const {
    return m_has_minor_representation || m_storage != ColumnOriented;
}
