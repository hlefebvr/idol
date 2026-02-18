//
// Created by Henri on 03/02/2026.
//
#include "idol/robust/optimizers/nested-branch-and-cut/Optimizers_NestedBranchAndCut.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

namespace idol::Bilevel {
    class OptimizerInterface;
}

idol::Optimizers::Robust::NestedBranchAndCut::NestedBranchAndCut(const Model& t_model,
                                                                 const idol::Robust::Description& t_robust_description,
                                                                 const idol::Bilevel::Description& t_bilevel_description,
                                                                 const OptimizerFactory& t_optimality_optimizer,
                                                                 const OptimizerFactory& t_feasibility_optimizer)
    : Algorithm(t_model),
      m_robust_description(t_robust_description),
      m_bilevel_description(t_bilevel_description),
      m_optimality_bilevel_optimizer(t_optimality_optimizer.clone()),
      m_feasibility_bilevel_optimizer(t_feasibility_optimizer.clone()) {

}

std::string idol::Optimizers::Robust::NestedBranchAndCut::name() const {
    return "nested-branch-and-cut";
}

idol::Robust::NBC::Formulation& idol::Optimizers::Robust::NestedBranchAndCut::get_formulation() {
    if (!m_formulation) {
        throw Exception("No formulation is available in this context.");
    }
    return *m_formulation;
}

const idol::Robust::NBC::Formulation& idol::Optimizers::Robust::NestedBranchAndCut::get_formulation() const {
    return const_cast<NestedBranchAndCut&>(*this).get_formulation();
}


void idol::Optimizers::Robust::NestedBranchAndCut::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    if (m_formulation) {
        return;
    }

    m_formulation = std::make_unique<idol::Robust::NBC::Formulation>(*this);

}

void idol::Optimizers::Robust::NestedBranchAndCut::hook_optimize() {

    auto& formulation = get_formulation();
    auto& model = formulation.model();
    const auto& bilevel_description = formulation.bilevel_description();
    const auto& first_stage_decisions = formulation.first_stage_decisions();

    m_optimality_bilevel_optimizer->as<Bilevel::OptimizerInterface>().set_bilevel_description(bilevel_description);
    m_feasibility_bilevel_optimizer->as<Bilevel::OptimizerInterface>().set_bilevel_description(bilevel_description);

    auto branch_and_bound = idol::BranchAndBound<Node>();
    branch_and_bound.with_root_node_info(Node(this));
    branch_and_bound.with_node_optimizer(*m_optimality_bilevel_optimizer);
    branch_and_bound.with_branching_rule(MostInfeasible(first_stage_decisions.begin(), first_stage_decisions.end()));
    branch_and_bound.with_node_selection_rule(BestBound());
    branch_and_bound.with_logs(get_param_logs());
    branch_and_bound.with_time_limit(get_param_time_limit());

    model.use(branch_and_bound);
    model.optimize();

    set_status(model.get_status());
    set_reason(model.get_reason());
    set_best_bound(model.get_best_bound());
    set_best_obj(model.get_best_obj());

}

void idol::Optimizers::Robust::NestedBranchAndCut::hook_after_optimize() {
    Algorithm::hook_after_optimize();
}

double idol::Optimizers::Robust::NestedBranchAndCut::get_var_primal(const Var& t_var) const {
    return get_formulation().model().get_var_primal(t_var);
}

double idol::Optimizers::Robust::NestedBranchAndCut::get_var_reduced_cost(const Var& t_var) const {
    return get_formulation().model().get_var_reduced_cost(t_var);
}

double idol::Optimizers::Robust::NestedBranchAndCut::get_var_ray(const Var& t_var) const {
    return get_formulation().model().get_var_ray(t_var);
}

double idol::Optimizers::Robust::NestedBranchAndCut::get_ctr_dual(const Ctr& t_ctr) const {
    return get_formulation().model().get_ctr_dual(t_ctr);
}

double idol::Optimizers::Robust::NestedBranchAndCut::get_ctr_farkas(const Ctr& t_ctr) const {
    return get_formulation().model().get_ctr_farkas(t_ctr);
}

unsigned idol::Optimizers::Robust::NestedBranchAndCut::get_n_solutions() const {
    return get_formulation().model().get_n_solutions();
}

unsigned idol::Optimizers::Robust::NestedBranchAndCut::get_solution_index() const {
    return get_formulation().model().get_solution_index();
}

void idol::Optimizers::Robust::NestedBranchAndCut::update() {
    hook_before_optimize();
}

void idol::Optimizers::Robust::NestedBranchAndCut::write(const std::string& t_name) {
    get_formulation().model().write(t_name);
}

void idol::Optimizers::Robust::NestedBranchAndCut::set_solution_index(unsigned t_index) {
    get_formulation().model().set_solution_index(t_index);
}

void idol::Optimizers::Robust::NestedBranchAndCut::add(const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::add(const Ctr& t_ctr) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::add(const QCtr& t_ctr) { m_formulation.reset(); }

void idol::Optimizers::Robust::NestedBranchAndCut::remove(const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::remove(const Ctr& t_ctr) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::remove(const QCtr& t_ctr) { m_formulation.reset(); }

void idol::Optimizers::Robust::NestedBranchAndCut::update_obj_sense() { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_obj() { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_rhs() { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_obj_constant() { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_mat_coeff(const Ctr& t_ctr, const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_ctr_type(const Ctr& t_ctr) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_ctr_rhs(const Ctr& t_ctr) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_var_type(const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_var_lb(const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_var_ub(const Var& t_var) { m_formulation.reset(); }
void idol::Optimizers::Robust::NestedBranchAndCut::update_var_obj(const Var& t_var) { m_formulation.reset(); }

void idol::Optimizers::Robust::NestedBranchAndCut::Node::save(const Model& t_original_formulation, const Model& t_model) {

    DefaultNodeInfo::save(t_original_formulation, t_model);

    auto& solution = primal_solution();

    if (solution.has_objective_value()) {
        solution.set_objective_value(-solution.objective_value());
    }

    if (const auto status = solution.status() ; status != Feasible && status != Optimal) {
         return;
    }

    // Checking feasibility
    const auto& nested_branch_and_cut = *m_parent;
    const auto& formulation = nested_branch_and_cut.get_formulation();
    const auto& feasibility_optimizer = nested_branch_and_cut.get_feasibility_bilevel_optimizer();

    AffExpr<Var> l1_norm;
    for (const auto& var : formulation.first_stage_decisions()) {
        const double value = t_model.get_var_primal(var);
        assert(is_integer(value, m_parent->get_tol_integer()));
        assert(t_model.get_var_type(var) == Binary);
        if (value < .5) {
            l1_norm += var;
        } else {
            l1_norm += 1 - var;
        }
    }

    auto model = t_model.copy();

    model.unuse();
    model.set_obj_expr((-l1_norm));

    if (true) {
        model.use(feasibility_optimizer);
    } else {
        model.use(Bilevel::PessimisticAsOptimistic(formulation.bilevel_description()) + feasibility_optimizer);
    }

    model.optimize();

    //std::cout << "Feasibility check obj. = " << model.get_best_obj() << std::endl;

    if (model.get_best_obj() > -.5) {
        std::cout << "SOLUTION FOUND !!" << std::endl;
        return;
    }

    auto sol = save_primal(model);
    //std::cout << "Computed L1-norm: " << evaluate(l1_norm, sol) << std::endl;

    bool is_feasible = true;
    for (const auto& var : formulation.first_stage_decisions()) {
        const double new_val = model.get_var_primal(var);
        const double old_val = t_model.get_var_primal(var);
        if (!is_zero(new_val - old_val, m_parent->get_tol_integer())) {
            is_feasible = false;
        }
        solution.set(var, (old_val + new_val) / 2.);
    }
    assert(!is_feasible || model.get_best_obj() > -.5);

    compute_sum_of_infeasibilities();

}

idol::Optimizers::Robust::NestedBranchAndCut::Node* idol::Optimizers::Robust::NestedBranchAndCut::Node::create_child() const {
    return new Node(m_parent);
}

idol::Optimizers::Robust::NestedBranchAndCut::Node* idol::Optimizers::Robust::NestedBranchAndCut::Node::clone() const {
    return new Node(*this);
}

idol::DefaultNodeUpdator<idol::Optimizers::Robust::NestedBranchAndCut::Node>* idol::Optimizers::Robust::NestedBranchAndCut::Node::create_updator(const Model& t_src_model, Model& t_relaxation) {
    return new DefaultNodeUpdator<Node>(t_src_model, t_relaxation);
}
