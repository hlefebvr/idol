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
#include "idol/robust/optimizers/deterministic/Deterministic.h"
#include "idol/robust/optimizers/affine-decision-rule/AffineDecisionRule.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/robust/optimizers/KAdaptabilityBranchAndBound/ScenarioBasedKAdaptabilityProblemSolver.h"
#include "idol/robust/optimizers/KAdaptabilityBranchAndBound/Optimizers_ScenarioBasedKAdaptabilityProblemSolver.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    /*****************/
    /* Read Instance */
    /*****************/

    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/Research/idol/examples/robust/flp-ccg.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    /****************************/
    /* Make Deterministic Model */
    /****************************/

    Model model(env);
    const auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    const auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "y");
    std::list<Ctr> second_stage_constraints;

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        const auto c = model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));
        second_stage_constraints.emplace_back(c);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        const auto c = model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) >= 1);
        second_stage_constraints.emplace_back(c);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            const auto c = model.add_ctr(y[i][j] <= x[i]);
            second_stage_constraints.emplace_back(c);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           instance.per_unit_transportation_cost(i, j) *
                                           instance.demand(j) *
                                           y[i][j]
                                )
                       )
    );

    /************************/
    /* Declare Second Stage */
    /************************/

    Bilevel::Description bilevel_description(env);
    for (const auto& y_ij : flatten<Var, 2>(y)) {
        bilevel_description.make_lower_level(y_ij);
    }
    for (const auto& ctr : second_stage_constraints) {
        bilevel_description.make_lower_level(ctr);
    }

    /**************************/
    /* Create Uncertainty Set */
    /**************************/

    Model uncertainty_set(env);
    const double Gamma = 2;
    const auto xi = uncertainty_set.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), idol_Sum(j, Range(n_customers), xi[i][j])) <= Gamma);

    /***********************/
    /* Declare Uncertainty */
    /***********************/

    Robust::Description robust_description(uncertainty_set);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            const auto c = model.add_ctr(y[i][j] <= 1);
            robust_description.set_uncertain_rhs(c, -xi[i][j]); // models y_ij <= 1 - xi_j
            bilevel_description.make_lower_level(c);
        }
    }

    const unsigned int K = 2;
    Robust::ScenarioBasedKAdaptabilityProblemSolver optimizer(bilevel_description, robust_description, K);
    optimizer.with_optimizer(Gurobi());

    model.use(optimizer);
    
    ////// START TEST DISJUNCTION //////
    std::vector<std::list<PrimalPoint>> disjunction(K);
    PrimalPoint point;
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        point.set(xi[0][j], 1);
        point.set(xi[1][j], 1);
    }
    disjunction[1].emplace_back(point);
    point.clear();
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        point.set(xi[2][j], 1);
        point.set(xi[4][j], 1);
    }
    disjunction[0].emplace_back(point);
    model.update();
    auto& impl = model.optimizer().as<Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver>();
    impl.set_uncertainty_disjunction(disjunction);
    ////// END TEST DISJUNCTION //////

    model.use(JuMP());

    model.optimize();

    for (unsigned int k = 0 ; k < K ; ++k) {
        model.set_solution_index(k);
        std::cout << model.get_solution_index() << " = " << std::endl;
        std::cout << save_primal(model) << std::endl;
    }

    return 0;
}
