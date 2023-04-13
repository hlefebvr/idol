#include "modeling.h"
#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/column-generation/ColumnGeneration.h"
#include "optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "optimizers/column-generation/IntegerMasterHeuristic.h"
#include "optimizers/callbacks/UserCutCallback.h"

class CoverCuts : public CuttingPlaneGenerator {

    void parse_ctr(const Model& t_model, const Ctr& t_ctr) {

        const auto ctr_type = t_model.get_ctr_type(t_ctr);

        if (ctr_type != LessOrEqual) { return; }

        auto& env = t_model.env();
        Model separation(env);
        const auto& row = t_model.get_ctr_row(t_ctr);
        double rhs = row.rhs().numerical();
        Expr lhs;
        std::list<Var> covered_vars;

        for (const auto& [var, constant] : row.linear()) {

            const auto var_type = t_model.get_var_type(var);

            if (var_type == Continuous) { return; }

            const double var_ub = t_model.get_var_ub(var);
            const double var_lb = t_model.get_var_lb(var);

            if (is_pos_inf(var_ub)) { return; }

            if (var_lb < -.5 || var_ub > 1.5) { return; }

            const double coeff = constant.numerical();

            if (coeff < 0) {
                rhs -= coeff * var_ub;
            } else {
                lhs += coeff * var;
                covered_vars.emplace_back(var);
                separation.add(var, TempVar(0, 1, Binary, Column()));
            }

        }

        separation.add_ctr(lhs >= rhs + 1);

        add_callback(
                UserCutCallback(separation, idol_Sum(var, covered_vars, !var * var) <= idol_Sum(var, covered_vars, !var) - 1)
                    .with_separation_solver(Gurobi())
                );
    }

public:

    void operator()(const Model &t_model) override {

        for (auto& ctr : t_model.ctrs()) {
            parse_ctr(t_model, ctr);
        }

    }

    [[nodiscard]] CuttingPlaneGenerator *clone() const override {
        return new CoverCuts(*this);
    }
};

int main(int t_argc, char** t_argv) {

    // Read instance
    const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance0.txt");
    //const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/n2/instance_n2_30__1.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    // Make model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> std_decomposition(env, "std_decomposition", MasterId);
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);
    Annotation<Ctr> decomposition2(env, "decomposition2", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(std_decomposition, i);
        capacity.set(decomposition, i/2);
        capacity.set(decomposition2, i % 2);
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    for (bool primal_heuristic : { true, false }) {

        for (bool branching_on_master: {true}) {

            for (bool farkas_pricing: {true, false}) {

                for (double smoothing: {0., .3, .5, .8}) {

                    model.use(

                            BranchAndBound<NodeInfo>()

                                    .with_node_solver(

                                            DantzigWolfeDecomposition(std_decomposition)

                                                    .with_master_solver(
                                                            Gurobi::ContinuousRelaxation()
                                                                //.with_infeasible_or_unbounded_info(true)
                                                    )

                                                    .with_pricing_solver(
                                                            Gurobi()
                                                    )

                                                    .with_log_level(Info, Magenta)

                                                    .with_farkas_pricing(farkas_pricing)

                                                    .with_dual_price_smoothing_stabilization(smoothing)

                                                    .with_branching_on_master(branching_on_master)

                                                    //.with_log_frequency(1)

                                                    //.with_iteration_count_limit(20)

                                    )

                                    .with_branching_rule(MostInfeasible())

                                    .with_node_selection_rule(BestBound())

                                    .conditional(primal_heuristic, [](auto &x) {
                                        x.with_callback(
                                                IntegerMasterHeuristic().with_solver(Gurobi().with_time_limit(20))
                                        );
                                    })

                                    .with_cutting_plane_generator(CoverCuts())

                                    .with_subtree_depth(1)

                                    .with_log_level(Trace, Blue)
                    );

                    std::cout << "RUNNING: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing
                              << std::endl;

                    model.optimize();

                    std::cout << "RESULT: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing
                              << std::endl;

                    std::cout << model.get_status() << std::endl;
                    std::cout << model.get_reason() << std::endl;
                    std::cout << model.get_best_obj() << std::endl;

                    return 0;

                }

            }

        }

    }

    return 0;
/*

    Env env;

    BlockModel<Var> model(env, 1);
    auto& subproblem = model.block(0);

    // Master variables
    Var theta(env, 0, Inf, Continuous, "theta");
    Var x(env, 0, Inf, Continuous, "x");

    model.add(theta);
    model.add(x);
    model.master().set_obj(2 * x + theta);

    // Subproblem variables
    auto lambda = Var::array(env, Dim<1>(2), 0, Inf, Continuous, "lambda");
    subproblem.model().add_array<Var, 1>(lambda);

    // Subproblem constraints
    Ctr c1(env, lambda[0] + 2 * lambda[1] <= 2.);
    subproblem.model().add(c1);

    Ctr c2(env, 2 * lambda[0] - lambda[1] <= 3.);
    subproblem.model().add(c2);

    // Generation pattern
    subproblem.generation_pattern() = Row(theta + (!lambda[0] + 3. * !lambda[1]) * x,  3. * !lambda[0] + 4. * !lambda[1]);

    // Set epigraph
    subproblem.set_aggregator(theta);

    subproblem.model().set_obj_sense(Maximize);

    model.build_opposite_axis();

    auto& rg = idol::set_optimizer<RowGeneration>(model);

    rg.set_master_backend<Gurobi>();
    rg.set_subproblem_backend<Gurobi>(0);


    model.optimize();
*/
    return 0;
}
