#include <iostream>
#include "idol/general/utils/SparseVector.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"
#include "idol/general/utils/GenerationPattern.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"

using namespace idol;

Model make_deterministic(const Model& t_model, const Robust::Description& t_description) {

    auto& env = t_model.env();
    Model result(env);

    for (const auto& var : t_model.vars()) {
        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);
        const auto type = t_model.get_var_type(var);
        result.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
    }

    for (const auto& ctr : t_model.ctrs()) {
        const auto& row = t_model.get_ctr_row(ctr);
        const auto rhs = t_model.get_ctr_rhs(ctr);
        const auto type = t_model.get_ctr_type(ctr);

        const auto& uncertain_mat_coeffs = t_description.uncertain_mat_coeffs(ctr);
        const auto& uncertain_rhs = t_description.uncertain_rhs(ctr);

        const bool has_lhs_uncertainty = !uncertain_mat_coeffs.is_zero(Tolerance::Sparsity);
        const bool has_rhs_uncertainty = !uncertain_rhs.is_zero(Tolerance::Sparsity);

        if (!has_lhs_uncertainty && !has_rhs_uncertainty) {
            result.add(ctr, TempCtr(LinExpr(row), type, rhs));
            continue;
        }

        if (type == Equal) {
            throw Exception("Cannot make equality constraints deterministic.");
        }

        const auto& uncertainty_set = t_description.uncertainty_set();

        QuadExpr<Var> objective = row;
        for (const auto& [var, uncertain_coeff] : row) {
            const auto& uncertain_mat_coeff = uncertain_mat_coeffs.get(var);
            objective +=  uncertain_mat_coeff * var;
        }
        for (const auto& [var, uncertain_coeff] : uncertain_rhs) {
            objective -= uncertain_coeff * var;
        }

        if (type == LessOrEqual) {
            objective *= -1;
        }

        Reformulators::KKT kkt(uncertainty_set,
                               objective,
                               [&](const Var& t_var) { return uncertainty_set.has(t_var); },
                               [&](const Ctr& t_ctr) { return uncertainty_set.has(t_ctr); },
                               [&](const QCtr& t_qvar) { return uncertainty_set.has(t_qvar); }
                               );
        kkt.set_prefix("robust_" + ctr.name() + "_");
        kkt.add_dual_variables(result);
        kkt.add_dual_constraints(result);

        auto dual_objective = kkt.get_dual_obj_expr();
        if (type == LessOrEqual) {
            dual_objective *= -1;
        }

        assert(!dual_objective.has_quadratic());

        result.add(ctr, TempCtr(
                                std::move(dual_objective.affine().linear()),
                                type,
                                -dual_objective.affine().constant())
                            );

    }

    return result;
}

int main(int t_argc, const char** t_argv) {

    Env env;

    const double Gamma = 3;

    // Generate parameters
    const auto filename = "/home/henri/Research/idol/tests/data/facility-location-problem/instance_F10_C20__0.txt";

    // Read instance
    const auto instance = idol::Problems::FLP::read_instance_1991_Cornuejols_et_al(filename);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::make_vector(env, Dim<1>(n_facilities), 0., 1., Continuous, 0., "x");
    auto y = Var::make_vector(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    Model uncertainty_set(env);
    const auto xi = uncertainty_set.add_vars(Dim<1>(n_facilities), 0., 1., Continuous, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), xi[i]) <= Gamma);

    Model model(env);

    model.add_vector<Var, 1>(x);
    model.add_vector<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    Robust::Description description(uncertainty_set);
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            description.make_stage_var(y[i][j], 1);
            auto activation = model.add_ctr(y[i][j] <= 1);
            description.set_uncertain_rhs(activation, -xi[i]);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers),
                                                                                                 instance.per_unit_transportation_cost(
                                                                                                         i, j) *
                                                                                                 instance.demand(j) *
                                                                                                 y[i][j])));

    std::cout << Robust::Description::View(model, description) << std::endl;

    auto deterministic = make_deterministic(model, description);

    deterministic.use(Gurobi());

    deterministic.optimize();

    std::cout << deterministic.get_status() << std::endl;
    std::cout << deterministic.optimizer().time().count() << std::endl;
    std::cout << save_primal(deterministic) << std::endl;

    return 0;
}
