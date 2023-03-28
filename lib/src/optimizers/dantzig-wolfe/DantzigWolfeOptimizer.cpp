//
// Created by henri on 24/03/23.
//
#include "optimizers/dantzig-wolfe/DantzigWolfeOptimizer.h"
#include "modeling/models/Model.h"
#include "modeling/expressions/operations/operators.h"
#include "modeling/objects/Versions.h"
#include "optimizers/column-generation/ColumnGenerationOptimizer.h"
#include "optimizers/dantzig-wolfe/DantzigWolfe.h"

DantzigWolfeOptimizer::DantzigWolfeOptimizer(const Annotation<Ctr, unsigned int> &t_decomposition,
                                             const OptimizerFactory &t_master_optimizer,
                                             const OptimizerFactory &t_subproblem_optimizer)
        : m_decomposition(t_decomposition),
          m_master_optimizer(t_master_optimizer.clone()),
          m_subproblem_optimizer(t_subproblem_optimizer.clone()) {

}

DantzigWolfeOptimizer::DantzigWolfeOptimizer(const DantzigWolfeOptimizer& t_src)
        : m_decomposition(t_src.m_decomposition),
          m_master_optimizer(t_src.m_master_optimizer->clone()),
          m_subproblem_optimizer(t_src.m_subproblem_optimizer->clone()) {

}

Optimizer *DantzigWolfeOptimizer::operator()(const Model &t_original_formulation) const {

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
        subproblems[i]->use(*m_subproblem_optimizer);
    }

    return new Optimizers::DantzigWolfe(t_original_formulation,
                                        m_decomposition,
                                        variable_flag,
                                        master,
                                        subproblems,
                                        generation_patterns);
}

DantzigWolfeOptimizer *DantzigWolfeOptimizer::clone() const {
    return new DantzigWolfeOptimizer(*this);
}

Annotation<Var, unsigned int> DantzigWolfeOptimizer::create_variable_flag(const Model &t_model, unsigned int *t_n_subproblem) const {

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

        const auto& row = t_model.get(Attr::Ctr::Row, ctr);

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

std::vector<Model*>
DantzigWolfeOptimizer::create_empty_subproblems(Env& t_env, unsigned int t_n_subproblems) const {

    std::vector<Model*> result;
    result.reserve(t_n_subproblems);

    for (unsigned int i = 0 ; i < t_n_subproblems ; ++i) {
        result.emplace_back(new Model(t_env));
    }

    return result;
}

std::vector<Column>
DantzigWolfeOptimizer::create_empty_generation_patterns(unsigned int t_n_subproblems) const {
    return std::vector<Column>(t_n_subproblems);
}

void DantzigWolfeOptimizer::dispatch_variables(const Annotation<Var, unsigned int> &t_variable_flag,
                                               const Model& t_original_formulation,
                                               Model *t_master,
                                               const std::vector<Model *> &t_subproblems) const {

    if (t_subproblems.empty()) {
        return;
    }

    for (const auto& var : t_original_formulation.vars()) {

        const unsigned int subproblem_id = var.get(t_variable_flag);
        const double lb = t_original_formulation.get(Attr::Var::Lb, var);
        const double ub = t_original_formulation.get(Attr::Var::Ub, var);
        const int type = t_original_formulation.get(Attr::Var::Type, var);

        auto* model = subproblem_id == MasterId ? t_master : t_subproblems[subproblem_id];

        if (!model->has(var)) {
            model->add(var, TempVar(lb, ub, type, Column()));
        }

    }

}

void DantzigWolfeOptimizer::dispatch_constraints(const Annotation<Var, unsigned int> &t_variable_flag,
                                                 const Model &t_original_formulation,
                                                 Model *t_master,
                                                 const std::vector<Model *> &t_subproblems,
                                                 std::vector<Column>& t_generation_patterns) const {

    if (t_subproblems.empty()) {
        return;
    }

    for (const auto& ctr : t_original_formulation.ctrs()) {

        const unsigned int subproblem_id = ctr.get(m_decomposition);
        const auto& row = t_original_formulation.get(Attr::Ctr::Row, ctr);
        const auto type = t_original_formulation.get(Attr::Ctr::Type, ctr);

        if (subproblem_id == MasterId) {
            dispatch_linking_constraint(t_variable_flag, ctr, row, type, t_master, t_subproblems, t_generation_patterns);
        } else {
            t_subproblems[subproblem_id]->add(ctr, TempCtr(Row(row), type));
        }

    }

}

void DantzigWolfeOptimizer::dispatch_linking_constraint(const Annotation<Var, unsigned int> &t_variable_flag,
                                                        const Ctr& t_ctr,
                                                        const Row &t_row,
                                                        int t_type,
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

void DantzigWolfeOptimizer::dispatch_objective(const Annotation<Var, unsigned int> &t_variable_flag,
                                               const Model &t_original_formulation, Model *t_master,
                                               std::vector<Column> &t_generation_patterns) const {

    const unsigned int n_subproblems = t_generation_patterns.size();

    const auto& objective = t_original_formulation.get(Attr::Obj::Expr);

    auto [master_obj, subproblem_patterns] = dispatch_linking_expression(t_variable_flag, objective.quadratic(), objective.linear(), t_master, n_subproblems);

    master_obj += objective.constant();

    t_master->set(Attr::Obj::Expr, std::move(master_obj));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        t_generation_patterns[i].set_obj(std::move(subproblem_patterns[i]));
    }

}

std::pair<Expr<Var, Var>, std::vector<Constant>>
DantzigWolfeOptimizer::dispatch_linking_expression(const Annotation<Var, unsigned int> &t_variable_flag,
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

        if (subproblem_id1 != MasterId || subproblem_id2 != MasterId) {
            throw Exception("Non-linear linking expressions are only allowed in the master problem.");
        }

        master_lhs += constant * var1 * var2;

    }

    return std::make_pair(
                std::move(master_lhs),
                std::move(subproblem_patterns)
            );

}

void DantzigWolfeOptimizer::add_convexity_constraints(Env& t_env, Model *t_master, std::vector<Column> &t_generation_patterns) const {

    const unsigned int n_subproblems = t_generation_patterns.size();

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        Ctr convexity_constraint(t_env, Equal, 1);
        t_master->add(convexity_constraint);
        t_generation_patterns[i].linear().set(convexity_constraint, 1);
    }

}
