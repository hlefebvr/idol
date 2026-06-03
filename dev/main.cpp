#include <iostream>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const unsigned int nb_arcs = 6;
    const std::vector<std::string> vertices{"s", "A", "B", "t"};
    const std::vector<std::pair<std::string, std::string>> arcs{{"s", "A"}, {"s", "B"}, {"A", "t"}, {"B", "t"}, {"A", "B"}, {"B", "A"}};
    const std::vector<double> costs{2., 3., 2., 2., 1., 1.};
    const std::vector<double> resources{4., 2., 3., 2., 2., 2.};
    const double capacity = 6.;

    Env env;
    Model model(env);

    const auto& x = model.add_vars(Dim<1>(nb_arcs), 0, 1, idol::VarType::Binary, 0., "x");

    for(const auto& i : vertices){
        LinExpr expr;
        for(unsigned int k = 0; k < nb_arcs ; ++k){
            if(arcs[k].first == i){
                expr += x[k];
            }
            else if(arcs[k].second == i){
                expr -= x[k];
            }
        }
        if(i == "s"){
            model.add_ctr(expr == 1, "flow_out_" + i);
        }
        else if(i == "t"){
            model.add_ctr(expr == -1, "flow_in_" + i);
        }
        else{
            model.add_ctr(expr == 0, "flow_null_" + i);
        }
    }

    model.add_ctr(idol_Sum(k, Range(nb_arcs), resources[k] * x[k]) <= capacity, "resource");

    model.set_obj_expr(idol_Sum(k, Range(nb_arcs), costs[k] * x[k]));

    std::cout <<  "Direct MILP example" << std::endl;

    Model direct_model(model.copy());

    direct_model.dump();

    direct_model.use(GLPK());

    direct_model.optimize();
    
    // Analyze the solution status
    const auto status = direct_model.get_status();
    const auto reason = direct_model.get_reason();
 
    std::cout << "Solution status: " << status << std::endl;
    std::cout << "Reason: " << reason << std::endl;
 
    if (status == Optimal || status == Feasible) {
 
        // Get the number of solutions in the solution pool
        const auto n_solutions = direct_model.get_n_solutions();
 
        std::cout << "Number of solutions: " << n_solutions << std::endl;
 
        // Print all solutions in the pool
        for (unsigned int i = 0 ; i < n_solutions ; ++i) {
            direct_model.set_solution_index(i);
            std::cout << "Solution " << i << std::endl;
            std::cout << save_primal(direct_model) << std::endl;
        }
 
    }

    std::cout <<  "Branch-and-Price reformulation" << std::endl;

    Model bap_model(model.copy());

    std::cout << MasterId << std::endl;
    Annotation decomposition(env, "decomposition", MasterId);

    for(const auto& ctr : bap_model.ctrs()){
        if(ctr.name().rfind("flow_", 0) == 0){
            ctr.set(decomposition, 0);
        }
    }
    for(const auto& ctr : bap_model.ctrs()) {
        std::cout
            << ctr.name()
            << " -> "
            << ctr.get(decomposition)
            << std::endl;
    }
    auto column_generation = DantzigWolfeDecomposition(decomposition);
    column_generation.with_master_optimizer(GLPK::ContinuousRelaxation());
    const auto subproblem_specifications = DantzigWolfe::SubProblem().add_optimizer(GLPK());
    column_generation.with_default_sub_problem_spec(subproblem_specifications);
    column_generation.with_infeasibility_strategy(DantzigWolfe::FarkasPricing());
    column_generation.with_hard_branching(false);
    column_generation.with_logs(true);

    auto branch_and_bound = BranchAndBound();
    branch_and_bound.with_branching_rule(MostInfeasible());
    branch_and_bound.with_node_selection_rule(BestBound());
    branch_and_bound.with_logs(true);

    const auto branch_and_price = branch_and_bound + column_generation;

    bap_model.use(branch_and_price);

    bap_model.optimize();
    
    // Analyze the solution status
    const auto bap_status = bap_model.get_status();
    const auto bap_reason = bap_model.get_reason();
 
    std::cout << "Solution status: " << bap_status << std::endl;
    std::cout << "Reason: " << bap_reason << std::endl;
 
    if (bap_status == Optimal || bap_status == Feasible) {
 
        // Get the number of solutions in the solution pool
        const auto n_solutions = bap_model.get_n_solutions();
 
        std::cout << "Number of solutions: " << n_solutions << std::endl;
 
        // Print all solutions in the pool
        for (unsigned int i = 0 ; i < n_solutions ; ++i) {
            bap_model.set_solution_index(i);
            std::cout << "Solution " << i << std::endl;
            std::cout << save_primal(bap_model) << std::endl;
        }
 
    }


    return 0;
}
