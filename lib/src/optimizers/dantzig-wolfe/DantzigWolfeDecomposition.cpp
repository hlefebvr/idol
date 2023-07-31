//
// Created by henri on 24/03/23.
//
#include "optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "modeling/models/Model.h"
#include "modeling/expressions/operations/operators.h"
#include "modeling/objects/Versions.h"
#include "optimizers/column-generation/ColumnGeneration.h"
#include "optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"

idol::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const Annotation<Ctr, unsigned int> &t_decomposition)
        : m_decomposition(t_decomposition) {

}

idol::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const DantzigWolfeDecomposition& t_src)
        : impl::OptimizerFactoryWithColumnGenerationParameters<DantzigWolfeDecomposition>(t_src),
          m_decomposition(t_src.m_decomposition),
          m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
          m_pricing_optimizer(t_src.m_pricing_optimizer ? t_src.m_pricing_optimizer->clone() : nullptr),
          m_branching_on_master(t_src.m_branching_on_master) {

}

idol::Optimizer *idol::DantzigWolfeDecomposition::operator()(const Model &t_original_formulation) const {

    if (!m_master_optimizer) {
        throw Exception("No master solver has been given, please call DantzigWolfeDecomposition::with_master_optimizer to configure.");
    }

    if (!m_pricing_optimizer) {
        throw Exception("No pricing solver has been given, please call DantzigWolfeDecomposition::with_pricing_optimizer to configure.");
    }

    auto& env = t_original_formulation.env();

    unsigned int n_subproblems;
    auto* master = new Model(env);

    auto variable_flag = create_variable_flag(t_original_formulation, &n_subproblems);

    auto subproblems = create_empty_subproblems(env, n_subproblems);
    auto generation_patterns = create_empty_generation_patterns(n_subproblems);

    dispatch_variables(variable_flag, t_original_formulation, master, subproblems);
    dispatch_constraints(variable_flag, t_original_formulation, master, subproblems, generation_patterns);
    dispatch_objective(variable_flag, t_original_formulation, master, generation_patterns);

    add_convexity_constraints(env, master, generation_patterns);

    master->use(*m_master_optimizer);

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        subproblems[i]->use(*m_pricing_optimizer);
    }

    auto* result = new Optimizers::DantzigWolfeDecomposition(t_original_formulation,
                                                             m_decomposition,
                                                             variable_flag,
                                                             master,
                                                             subproblems,
                                                             generation_patterns,
                                                             *m_pricing_optimizer);

    this->handle_default_parameters(result);
    this->handle_column_generation_parameters(result);

    if (m_branching_on_master.has_value()) {
        result->set_branching_on_master(m_branching_on_master.value());
    }

    return result;
}

idol::DantzigWolfeDecomposition *idol::DantzigWolfeDecomposition::clone() const {
    return new DantzigWolfeDecomposition(*this);
}

idol::Annotation<idol::Var, unsigned int> idol::DantzigWolfeDecomposition::create_variable_flag(const Model &t_model, unsigned int *t_n_subproblem) const {

    Annotation<Var, unsigned int> result(t_model.env(), "variable_flag", MasterId);

    *t_n_subproblem = 0;

    const auto set_flag = [&result](const Var& t_var, unsigned int t_subproblem_id) {

        if (t_subproblem_id == MasterId) { return; }

        const unsigned int current_subproblem_id = t_var.get(result);

        if (current_subproblem_id != MasterId && current_subproblem_id != t_subproblem_id) {
            throw Exception("Infeasible decomposition given.");
        }

        t_var.set(result, t_subproblem_id);
    };

    bool has_at_least_one_subproblem = false;

    for (const auto& ctr : t_model.ctrs()) {

        const unsigned int subproblem_id = ctr.get(m_decomposition);

        if (subproblem_id != MasterId && subproblem_id >= *t_n_subproblem) {
            has_at_least_one_subproblem = true;
            *t_n_subproblem = subproblem_id;
        }

        const auto& row = t_model.get_ctr_row(ctr);

        for (const auto& [var, constant] : row.linear()) {
            set_flag(var, subproblem_id);
        }

        for (const auto& [var1, var2, constant] : row.quadratic()) {
            set_flag(var1, subproblem_id);
            set_flag(var2, subproblem_id);
        }

    }

    if (has_at_least_one_subproblem) {
        *t_n_subproblem += 1;
    }

    return result;
}

std::vector<idol::Model*>
idol::DantzigWolfeDecomposition::create_empty_subproblems(Env& t_env, unsigned int t_n_subproblems) const {

    std::vector<Model*> result;
    result.reserve(t_n_subproblems);

    for (unsigned int i = 0 ; i < t_n_subproblems ; ++i) {
        result.emplace_back(new Model(t_env));
    }

    return result;
}

std::vector<idol::Column>
idol::DantzigWolfeDecomposition::create_empty_generation_patterns(unsigned int t_n_subproblems) const {
    return std::vector<Column>(t_n_subproblems);
}

void idol::DantzigWolfeDecomposition::dispatch_variables(const Annotation<Var, unsigned int> &t_variable_flag,
                                               const Model& t_original_formulation,
                                               Model *t_master,
                                               const std::vector<Model *> &t_subproblems) const {

    for (const auto& var : t_original_formulation.vars()) {

        const unsigned int subproblem_id = var.get(t_variable_flag);
        const double lb = t_original_formulation.get_var_lb(var);
        const double ub = t_original_formulation.get_var_ub(var);
        const VarType type = t_original_formulation.get_var_type(var);

        auto* model = subproblem_id == MasterId ? t_master : t_subproblems[subproblem_id];

        if (!model->has(var)) {
            model->add(var, TempVar(lb, ub, type, Column()));
        }

    }

}

void idol::DantzigWolfeDecomposition::dispatch_constraints(const Annotation<Var, unsigned int> &t_variable_flag,
                                                 const Model &t_original_formulation,
                                                 Model *t_master,
                                                 const std::vector<Model *> &t_subproblems,
                                                 std::vector<Column>& t_generation_patterns) const {

    if (t_subproblems.empty()) {
        return;
    }

    for (const auto& ctr : t_original_formulation.ctrs()) {

        const unsigned int subproblem_id = ctr.get(m_decomposition);
        const auto& row = t_original_formulation.get_ctr_row(ctr);
        const auto type = t_original_formulation.get_ctr_type(ctr);

        if (subproblem_id == MasterId) {
            dispatch_linking_constraint(t_variable_flag, ctr, row, type, t_master, t_subproblems, t_generation_patterns);
        } else {
            t_subproblems[subproblem_id]->add(ctr, TempCtr(Row(row), type));
        }

    }

}

void idol::DantzigWolfeDecomposition::dispatch_linking_constraint(const Annotation<Var, unsigned int> &t_variable_flag,
                                                        const Ctr& t_ctr,
                                                        const Row &t_row,
                                                        CtrType t_type,
                                                        Model *t_master,
                                                        const std::vector<Model *> &t_subproblems,
                                                        std::vector<Column> &t_generation_patterns) const {

    const unsigned int n_subproblems = t_subproblems.size();

    auto [master_lhs, subproblem_patterns] = dispatch_linking_expression(t_variable_flag, t_row.quadratic(), t_row.linear(), t_master, n_subproblems);

    t_master->add(t_ctr, TempCtr(Row(std::move(master_lhs), t_row.rhs()), t_type));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        t_generation_patterns[i].linear().set(t_ctr, std::move(subproblem_patterns[i]));
    }

}

void idol::DantzigWolfeDecomposition::dispatch_objective(const Annotation<Var, unsigned int> &t_variable_flag,
                                               const Model &t_original_formulation, Model *t_master,
                                               std::vector<Column> &t_generation_patterns) const {

    const unsigned int n_subproblems = t_generation_patterns.size();

    const auto& objective = t_original_formulation.get_obj_expr();

    auto [master_obj, subproblem_patterns] = dispatch_linking_expression(t_variable_flag, objective.quadratic(), objective.linear(), t_master, n_subproblems);

    master_obj += objective.constant();

    t_master->set_obj_expr(std::move(master_obj));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        t_generation_patterns[i].set_obj(std::move(subproblem_patterns[i]));
    }

}

std::pair<idol::Expr<idol::Var, idol::Var>, std::vector<idol::Constant>>
idol::DantzigWolfeDecomposition::dispatch_linking_expression(const Annotation<Var, unsigned int> &t_variable_flag,
                                                   const QuadExpr<Var, Var> &t_quad,
                                                   const LinExpr<Var> &t_lin,
                                                   Model *t_master,
                                                   unsigned int t_n_subproblems) const {

    Expr<Var, Var> master_lhs;
    std::vector<Constant> subproblem_patterns(t_n_subproblems);

    for (const auto& [var, constant] : t_lin) {

        const unsigned int subproblem_id = var.get(t_variable_flag);

        if (subproblem_id == MasterId) {
            master_lhs += constant * var;
            continue;
        }

        if (!constant.is_numerical()) {
            throw Exception("Cannot have linking expression with subproblem's variable associated to a parametrized coefficient.");
        }

        subproblem_patterns[subproblem_id] += constant.numerical() * !var;

    }

    for (const auto& [var1, var2, constant] : t_quad) {

        const unsigned int subproblem_id1 = var1.get(t_variable_flag);
        const unsigned int subproblem_id2 = var2.get(t_variable_flag);

        if (subproblem_id1 != subproblem_id2) {
            throw Exception("Cannot mix block-variables in quadratic expressions.");
        }

        if (subproblem_id1 == MasterId) {
            master_lhs += constant * var1 * var2;
            continue;
        }

        if (!constant.is_numerical()) {
            throw Exception("Cannot have linking expression with subproblem's variable associated to a parametrized coefficient.");
        }

        subproblem_patterns[subproblem_id1] += constant.numerical() * (!var1 * !var2);

    }

    return std::make_pair(
                std::move(master_lhs),
                std::move(subproblem_patterns)
            );

}

void idol::DantzigWolfeDecomposition::add_convexity_constraints(Env& t_env, Model *t_master, std::vector<Column> &t_generation_patterns) const {

    const unsigned int n_subproblems = t_generation_patterns.size();

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        Ctr convexity_constraint(t_env, Equal, 1);
        t_master->add(convexity_constraint);
        t_generation_patterns[i].linear().set(convexity_constraint, 1);
    }

}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_pricing_optimizer(const OptimizerFactory &t_pricing_optimizer) {

    if (m_pricing_optimizer) {
        throw Exception("A pricing solver has already been set.");
    }

    m_pricing_optimizer.reset(t_pricing_optimizer.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_master_optimizer(const OptimizerFactory &t_master_optimizer) {

    if (m_master_optimizer) {
        throw Exception("A master solver has already been set.");
    }

    m_master_optimizer.reset(t_master_optimizer.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_branching_on_master(bool t_value) {

    if (m_branching_on_master.has_value()) {
        throw Exception("Branching on master setting has already been given.");
    }

    m_branching_on_master = t_value;

    return *this;
}
