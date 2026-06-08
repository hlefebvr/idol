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
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaContext.h"

using namespace idol;

void create_rspp_small_model(Env& env, Model& model){

    const unsigned int nb_arcs = 6;
    const std::vector<std::string> vertices{"s", "A", "B", "t"};
    const std::vector<std::pair<std::string, std::string>> arcs{{"s", "A"}, {"s", "B"}, {"A", "t"}, {"B", "t"}, {"A", "B"}, {"B", "A"}};
    const std::vector<double> costs{2., 3., 2., 2., 1., 1.};
    const std::vector<double> resources{4., 2., 3., 2., 2., 2.};
    const double capacity = 6.;

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

    model.dump();
}

void create_rspp_big_model(Env& env, Model& model){

    const std::vector<std::string> vertices = {
        "s",
        "A1","A2","A3",
        "B1","B2","B3",
        "C1","C2","C3",
        "t"
    };

    std::vector<std::pair<std::string,std::string>> arcs {
        {"s", "A1"}, {"s", "A2"}, {"s", "A3"},
        {"s", "B1"}, {"s", "B2"}, {"s", "B3"},

        {"A1", "B1"}, {"A1", "B2"}, {"A1", "B3"},
        {"A2", "B1"}, {"A2", "B2"}, {"A2", "B3"},
        {"A3", "B1"}, {"A3", "B2"}, {"A2", "B3"},

        {"B1", "C1"}, {"B1", "C2"}, {"B1", "C3"},
        {"B2", "C1"}, {"B2", "C2"}, {"B2", "C3"},
        {"B3", "C1"}, {"B3", "C2"}, {"B2", "C3"},

        {"C1", "t"}, {"C2", "t"}, {"C3", "t"}
    };

    std::vector<double> costs {
        2, 3, 1, 
        2, 3, 1,

        2, 1, 2, 
        1, 2, 1, 
        2, 1, 2,

        2, 1, 2, 
        1, 2, 1, 
        2, 1, 2,

        2, 3, 1
    };

    std::vector<double> resources {
        3, 4, 5, 
        2, 3, 4,

        1, 2, 3, 
        2, 3, 1, 
        3, 1, 2,

        1, 2, 3, 
        2, 3, 1, 
        3, 1, 2,

        5, 2, 3
    };

    const double capacity = 10.;

    const auto& x = model.add_vars(Dim<1>(arcs.size()), 0, 1, idol::VarType::Binary, 0., "x");

    for(const auto& i : vertices){
        LinExpr expr;
        for(unsigned int k = 0; k < arcs.size() ; ++k){
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

    model.add_ctr(idol_Sum(k, Range(arcs.size()), resources[k] * x[k]) <= capacity, "resource");

    model.set_obj_expr(idol_Sum(k, Range(arcs.size()), costs[k] * x[k]));

    model.dump();
}

void print_solution_status(Model& model){

    // Analyze the solution status
    const auto status = model.get_status();
    const auto reason = model.get_reason();
 
    std::cout << "Solution status: " << status << std::endl;
    std::cout << "Reason: " << reason << std::endl;
 
    if (status == Optimal || status == Feasible) {
 
        // Get the number of solutions in the solution pool
        const auto n_solutions = model.get_n_solutions();
 
        std::cout << "Number of solutions: " << n_solutions << std::endl;
 
        // Print all solutions in the pool
        for (unsigned int i = 0 ; i < n_solutions ; ++i) {
            model.set_solution_index(i);
            std::cout << "Solution " << i << std::endl;
            std::cout << save_primal(model) << std::endl;
        }
    }
}

void direct_solve(Model& model){

    std::cout <<  "Direct GLPK solving" << std::endl;

    Model direct_model(model.copy());

    direct_model.use(GLPK());
    direct_model.optimize();
    
    print_solution_status(direct_model);
}

void branch_and_price_solve(Env& env, Model& model){

    std::cout <<  "Branch-and-Price GLPK + GLPK solving" << std::endl;

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

    print_solution_status(bap_model); 
}

void direct_solve_using_lambda_function(Model& model){

    std::cout <<  "Direct lambda function solving" << std::endl;

    Model direct_model(model.copy());

    const auto lambda = [](LambdaContext& t_ctx){
        std::cout << "Begin lambda" << std::endl; 

        auto copy = t_ctx.get_model().copy();
        copy.use(GLPK());
        copy.optimize();
        print_solution_status(copy); 

        t_ctx.set_status(Optimal);

        std::cout <<  "End lambda" << std::endl; 
    };

    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    direct_model.use(LambdaOptimizer(lambda));
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    direct_model.optimize();
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    
}

int main(int t_argc, const char** t_argv) {

    if (t_argc != 3) {
        std::cerr << "Usage: " << t_argv[0] << " [direct/bap]\n";
        return 1;
    }

    std::string model_size = t_argv[1];
    Env env;
    auto model = Model(env);

    if (model_size == "small"){
        create_rspp_small_model(env, model);
    }
    else if (model_size == "big"){
        create_rspp_big_model(env, model);
    }


    std::string mode = t_argv[2];
    if (mode == "direct"){
        direct_solve(model);
    }
    else if (mode == "bap"){
        branch_and_price_solve(env, model);
    }
    else if (mode == "lambda"){
        direct_solve_using_lambda_function(model);
    }
    
    return 0;
}
