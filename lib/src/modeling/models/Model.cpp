//
// Created by henri on 27/01/23.
//
#include "idol/modeling//models/Model.h"
#include "idol/modeling//objects/Env.h"

idol::Model::Model(Env &t_env) : m_env(t_env), m_id(t_env.create_model_id()) {}


idol::Model::Model(idol::Env &t_env, idol::ObjectiveSense t_sense) : Model(t_env) {
    set_obj_sense(t_sense);
}


idol::Model::Model(idol::Model && t_src) noexcept
    : m_env(t_src.m_env),
      m_id(t_src.m_id),
      m_sense(t_src.m_sense),
      m_objective(std::move(t_src.m_objective)),
      m_rhs(std::move(t_src.m_rhs)),
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
    m_env.create_version(*this,
                         t_var,
                         m_variables.size(),
                         t_temp_var.lb(),
                         t_temp_var.ub(),
                         t_temp_var.type(),
                         std::move(t_temp_var.column())
    );
    m_variables.emplace_back(t_var);

    if (has_optimizer()) {
        optimizer().add(t_var);
    }

    add_column_to_rows(t_var);
}

void idol::Model::add(const Var &t_var) {
    const auto& default_version = m_env[t_var];
    add(t_var, TempVar(
             default_version.lb(),
             default_version.ub(),
             default_version.type(),
             Column(default_version.column())
    ));
}

void idol::Model::remove(const Var &t_var) {

    if (has_optimizer()) {
        optimizer().remove(t_var);
    }

    remove_column_from_rows(t_var);
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

    remove_row_from_columns(t_ctr);
    const auto index = m_env.version(*this, t_ctr).index();
    m_env.version(*this, m_constraints.back()).set_index(index);
    m_constraints[index] = m_constraints.back();
    m_constraints.pop_back();
    m_env.remove_version(*this, t_ctr);
}

void idol::Model::add(const Ctr &t_ctr, TempCtr t_temp_ctr) {
    m_env.create_version(*this, t_ctr, m_constraints.size(), std::move(t_temp_ctr));
    m_constraints.emplace_back(t_ctr);

    if (has_optimizer()) {
        optimizer().add(t_ctr);
    }

    add_row_to_columns(t_ctr);
}

void idol::Model::add(const Ctr &t_ctr) {
    const auto& default_version = m_env[t_ctr];
    add(t_ctr, TempCtr(
            Row(default_version.row()),
            default_version.type())
        );
}

idol::Expr<idol::Var> &idol::Model::access_obj() {
    return m_objective;
}

idol::LinExpr<idol::Ctr> &idol::Model::access_rhs() {
    return m_rhs;
}

idol::Column &idol::Model::access_column(const Var &t_var) {
    return m_env.version(*this, t_var).column();
}

idol::Row &idol::Model::access_row(const Ctr &t_ctr) {
    return m_env.version(*this, t_ctr).row();
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

const idol::Constant &idol::Model::get_mat_coeff(const Ctr &t_ctr, const Var &t_var) const {
    return m_env.version(*this, t_ctr).row().linear().get(t_var);
}

const idol::Row &idol::Model::get_ctr_row(const Ctr &t_ctr) const {
    return m_env.version(*this, t_ctr).row();
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

const idol::Column &idol::Model::get_var_column(const Var &t_var) const {
    return m_env.version(*this, t_var).column();
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

    for (const auto& var : t_src.vars()) {
        add(var, TempVar(
                t_src.get_var_lb(var),
                t_src.get_var_ub(var),
                t_src.get_var_type(var),
                Column()
        ));
    }

    for (const auto& ctr : t_src.ctrs()) {
        add(ctr, TempCtr(
                Row(t_src.get_ctr_row(ctr)),
                t_src.get_ctr_type(ctr)
        ));
    }

    set_obj_sense(t_src.get_obj_sense());
    set_obj_expr(t_src.get_obj_expr());

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

    replace_objective(Expr<Var, Var>(t_objective));

    if (has_optimizer()) {
        optimizer().update_obj();
    }

}

void idol::Model::set_rhs_expr(const LinExpr<Ctr> &t_rhs) {
    set_rhs_expr(LinExpr<Ctr>(t_rhs));
}

void idol::Model::set_rhs_expr(LinExpr<Ctr> &&t_rhs) {

    replace_right_handside(LinExpr<Ctr>(t_rhs));

    if (has_optimizer()) {
        optimizer().update_rhs();
    }

}

void idol::Model::set_obj_const(const Constant &t_constant) {
    set_obj_const(Constant(t_constant));
}

void idol::Model::set_obj_const(Constant &&t_constant) {

    m_objective.constant() = std::move(t_constant);

    if (has_optimizer()) {
        optimizer().update_obj_constant();
    }

}

void idol::Model::set_mat_coeff(const Ctr &t_ctr, const Var &t_var, const Constant &t_coeff) {
    set_mat_coeff(t_ctr, t_var, Constant(t_coeff));
}

void idol::Model::set_mat_coeff(const Ctr &t_ctr, const Var &t_var, Constant &&t_coeff) {

    update_matrix_coefficient(t_ctr, t_var, std::move(t_coeff));

    if (has_optimizer()) {
        optimizer().update();
        optimizer().update_mat_coeff(t_ctr, t_var);
    }

}

void idol::Model::set_ctr_rhs(const Ctr &t_ctr, const Constant &t_rhs) {
    set_ctr_rhs(t_ctr, Constant(t_rhs));
}

void idol::Model::set_ctr_rhs(const Ctr &t_ctr, Constant &&t_rhs) {

    add_to_rhs(t_ctr, Constant(t_rhs));

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

void idol::Model::set_ctr_row(const Ctr &t_ctr, const Row &t_row) {
    set_ctr_row(t_ctr, Row(t_row));
}

void idol::Model::set_ctr_row(const Ctr &t_ctr, Row &&t_row) {

    remove_row_from_columns(t_ctr);

    m_env.version(*this, t_ctr).row() = std::move(t_row);

    add_row_to_columns(t_ctr);

    if (has_optimizer()) {
        throw Exception("Updating row is not implemented.");
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

void idol::Model::set_var_obj(const Var &t_var, const Constant &t_obj) {
    set_var_obj(t_var, Constant(t_obj));
}

void idol::Model::set_var_obj(const Var &t_var, Constant &&t_obj) {

    add_to_obj(t_var, std::move(t_obj));

    if (has_optimizer()) {
        optimizer().update_var_obj(t_var);
    }

}

void idol::Model::set_var_column(const Var &t_var, const Column &t_column) {
    set_var_column(t_var, Column(t_column));
}

void idol::Model::set_var_column(const Var &t_var, Column &&t_column) {

    remove_column_from_rows(t_var);

    m_env.version(*this, t_var).column() = std::move(t_column);

    add_column_to_rows(t_var);

    if (has_optimizer()) {
        throw Exception("Updating column is not implemented.");
    }

}

idol::Var idol::Model::add_var(double t_lb, double t_ub, VarType t_type, std::string t_name) {
    return add_var(t_lb, t_ub, t_type, Column(), std::move(t_name));
}

idol::Var idol::Model::add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name) {
    Var result(m_env, t_lb, t_ub, t_type, std::move(t_column), std::move(t_name));
    add(result);
    return result;
}

idol::Ctr idol::Model::add_ctr(Row &&t_row, CtrType t_type, std::string t_name) {
    return add_ctr(TempCtr(std::move(t_row), t_type), std::move(t_name));
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

void idol::Model::scale_to_integers(unsigned int t_n_digits) {

    for (const auto& ctr : m_constraints) {
        access_row(ctr).scale_to_integers(t_n_digits);
    }

}

idol::Model idol::Model::copy() const {
    return Model(*this);
}

idol::Model idol::Model::fix(const idol::Solution::Primal &t_primals) const {

    Model result(m_env);

    for (const auto& var : vars()) {
        result.add(var, TempVar(
                get_var_lb(var),
                get_var_ub(var),
                get_var_type(var),
                Column()
        ));
    }

    for (const auto& ctr : ctrs()) {
        result.add(ctr, TempCtr(
                std::move(get_ctr_row(ctr).fix(t_primals)),
                get_ctr_type(ctr)
        ));
    }

    result.set_obj_sense(get_obj_sense());
    result.set_obj_expr(get_obj_expr().fix(t_primals));

    if (m_optimizer_factory) {
        result.use(*m_optimizer_factory);
    }

    return result;
}
