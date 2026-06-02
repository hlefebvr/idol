#include <iostream>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {
    std::cout <<  "CMA" << std::endl;

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
            model.add_ctr(expr == 1);
        }
        else if(i == "t"){
            model.add_ctr(expr == -1);
        }
        else{
            model.add_ctr(expr == 0);
        }
    }
    model.add_ctr(idol_Sum(k, Range(nb_arcs), resources[k] * x[k]) <= capacity);

    model.set_obj_expr(idol_Sum(k, Range(nb_arcs), costs[k] * x[k]));

    model.dump();

    model.use(GLPK());

    model.optimize();
    
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

    return 0;
}
