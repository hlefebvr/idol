//
// Created by henri on 27/01/23.
//
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
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

    // TODO sort and reduce by index in the model

    throw_if_unknown_object(t_temp_var.column());

    m_env.create_version(*this,
                         t_var,
                         m_variables.size(),
                         t_temp_var.lb(),
                         t_temp_var.ub(),
                         t_temp_var.type(),
                         t_temp_var.obj(),
                         std::move(t_temp_var.column())
    );

    m_variables.emplace_back(t_var);

    if (has_optimizer()) {
        optimizer().add(t_var);
    }

    const auto& column = get_var_column(t_var);
    m_objective += t_temp_var.obj() * t_var;

    if (m_storage == ColumnOriented && !m_has_minor_representation) {
        return;
    }

    add_column_to_rows(t_var);

    if (m_storage == RowOriented) {
        m_env.version(*this, t_var).reset_column();
    }

}

void idol::Model::add_column_to_rows(const idol::Var &t_var) {

    const auto& column = get_var_column(t_var);

    for (const auto& [ctr, constant] : column) {
        auto& version = m_env.version(*this, ctr);
        if (version.has_row()) {
            version.lhs().push_back(t_var, constant);
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

    // remove_column_from_rows(t_var);
    throw Exception("Model. Work in progress...");
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

    //remove_row_from_columns(t_ctr);
    throw Exception("Model. Work in progress...");
    const auto index = m_env.version(*this, t_ctr).index();
    m_env.version(*this, m_constraints.back()).set_index(index);
    m_constraints[index] = m_constraints.back();
    m_constraints.pop_back();
    m_env.remove_version(*this, t_ctr);
}

void idol::Model::add(const Ctr &t_ctr, TempCtr t_temp_ctr) {

    throw_if_unknown_object(t_temp_ctr.lhs());

    m_env.create_version(*this,
                         t_ctr,
                         m_constraints.size(),
                         std::move(t_temp_ctr.lhs()),
                         t_temp_ctr.type(),
                         t_temp_ctr.rhs());
    m_constraints.emplace_back(t_ctr);

    if (has_optimizer()) {
        optimizer().add(t_ctr);
    }

    const auto& lhs = get_ctr_row(t_ctr);

    if (m_storage == RowOriented && !m_has_minor_representation) {
        return;
    }

    add_row_to_columns(t_ctr);

    if (m_storage == ColumnOriented) {
        m_env.version(*this, t_ctr).reset_row();
    }

}

void idol::Model::add_row_to_columns(const idol::Ctr &t_ctr) {

    const auto& lhs = get_ctr_row(t_ctr);

    for (const auto& [var, constant] : lhs) {
        auto& version = m_env.version(*this, var);
        if (version.has_column()) {
            version.column().push_back(t_ctr, constant);
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

const idol::Expr<idol::Var, idol::Var> &idol::Model::get_obj_expr() const {
    return m_objective;
}

const idol::LinExpr<idol::Ctr> &idol::Model::get_rhs_expr() const {
    return m_rhs;
}

double idol::Model::get_mat_coeff(const Ctr &t_ctr, const Var &t_var) const {
    if (m_storage == Storage::ColumnOriented) {
        return m_env.version(*this, t_var).column().get(t_ctr);
    }
    return m_env.version(*this, t_ctr).lhs().get(t_var);
}

idol::LinExpr<idol::Var> idol::Model::get_ctr_row(const Ctr &t_ctr) const {
    auto& version = m_env.version(*this, t_ctr);

    if (!version.has_row()) {
        const_cast<Model*>(this)->build_row(t_ctr);
        m_has_minor_representation = true;
    }

    return version.lhs();
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

    m_objective = std::move(t_objective);

    if (has_optimizer()) {
        optimizer().update_obj();
    }

}

void idol::Model::set_rhs_expr(const LinExpr<Ctr> &t_rhs) {
    set_rhs_expr(LinExpr<Ctr>(t_rhs));
}

void idol::Model::set_rhs_expr(LinExpr<Ctr> &&t_rhs) {

    throw_if_unknown_object(t_rhs);

    //replace_right_handside(LinExpr<Ctr>(t_rhs));
    throw Exception("Model. Work in progress...");

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

    throw Exception("Model. Work in progress...");
    // update_matrix_coefficient(t_ctr, t_var, t_coeff);

    if (has_optimizer()) {
        optimizer().update();
        optimizer().update_mat_coeff(t_ctr, t_var);
    }

}

void idol::Model::set_ctr_rhs(const Ctr &t_ctr, double t_rhs) {

    throw Exception("Model. Work in progress...");
    // add_to_rhs(t_ctr, t_rhs);

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

    /*
    Row old_row;

    // TODO: Do this properly
    if (has_optimizer()) {
        old_row = get_ctr_row(t_ctr);

        if (!t_row.quadratic().empty()) {
            throw Exception("Updating a quadratic constraint lhs is not implemented.");
        }

    }
    */
    throw Exception("Model. Work in progress...");
    //remove_row_from_columns(t_ctr);

    //m_env.version(*this, t_ctr).row() = std::move(t_row);

    throw Exception("Model. Work in progress...");
    //add_row_to_columns(t_ctr);

    // TODO: Do this properly
    /*
    if (has_optimizer()) {

        optimizer().update();

        for (const auto& [var, constant] : old_row.linear()) {
            optimizer().update_mat_coeff(t_ctr, var);
        }

        for (const auto& [var, constant] : get_ctr_row(t_ctr)) {
            optimizer().update_mat_coeff(t_ctr, var);
        }

        optimizer().update_ctr_rhs(t_ctr);
    }
    */
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

    throw Exception("Model. Work in progress...");
    //add_to_obj(t_var, std::move(t_obj));

    if (has_optimizer()) {
        optimizer().update_var_obj(t_var);
    }

}

void idol::Model::set_var_column(const Var &t_var, const LinExpr<Ctr> &t_column) {
    set_var_column(t_var, LinExpr<Ctr>(t_column));
}

void idol::Model::set_var_column(const Var &t_var, LinExpr<Ctr> &&t_column) {

    throw Exception("Model. Work in progress...");
    //remove_column_from_rows(t_var);

    m_env.version(*this, t_var).column() = std::move(t_column);

    throw Exception("Model. Work in progress...");
    //add_column_to_rows(t_var);

    if (has_optimizer()) {
        throw Exception("Updating column is not implemented.");
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

    LinExpr<Var> lhs;
    for (const auto& var : vars()) {
        const auto& column = get_var_column(var);
        const double value = column.get(t_ctr);
        lhs.push_back(var, value);
    }

    lhs.sparsify();

    m_env.version(*this, t_ctr).set_row(std::move(lhs));

}

void idol::Model::build_column(const idol::Var &t_var) {

    LinExpr<Ctr> column;
    for (const auto& ctr : ctrs()) {
        const auto& lhs = get_ctr_row(ctr);
        column.push_back(ctr, lhs.get(t_var));
    }

    column.sparsify();

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

    t_os << m_objective << '\n';

    t_os << "RHS:\n";
    t_os << "----\n";

    t_os << m_rhs << '\n';
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
                const auto& linear = version.lhs();
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
    } else { [[unlikely]]
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
        for (const auto& ctr : ctrs()) {
            m_env.version(*this, ctr).reset_row();
        }
        return;
    }

    if (m_storage == RowOriented) {
        for (const auto& var : vars()) {
            m_env.version(*this, var).reset_column();
        }
        return;
    }

}

void idol::Model::build_rows() {
    for (const auto& ctr : ctrs()) {
        if (!m_env.version(*this, ctr).has_row()) {
            build_row(ctr);
        }
    }
}

void idol::Model::build_columns() {
    for (const auto& var : vars()) {
        if (!m_env.version(*this, var).has_column()) {
            build_column(var);
        }
    }
}

double idol::Model::get_var_obj(const idol::Var &t_var) const {
    return get_obj_expr().linear().get(t_var);
}

double idol::Model::get_ctr_rhs(const idol::Ctr &t_ctr) const {
    return get_rhs_expr().get(t_ctr);
}
