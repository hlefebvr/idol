//
// Created by Charlotte on 08/07/2026.
//
#include <iostream>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaContext.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace idol;

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

int main() {

    /**********************/
    /* Create boost graph */
    /**********************/

    using Graph = boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            boost::no_property,
            boost::property<boost::edge_weight_t, double>
        >;

    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph g;

    const auto& s = add_vertex(g);
    const auto& A = add_vertex(g);
    const auto& B = add_vertex(g);
    const auto& t = add_vertex(g);

    add_edge(s, A, 2.0, g);
    add_edge(s, B, 3.0, g);
    add_edge(A, t, 2.0, g);
    add_edge(B, t, 2.0, g);
    add_edge(A, B, 1.0, g);
    add_edge(B, A, 1.0, g);

    /**********************/
    /* Create IDOL model  */
    /**********************/

    Env env;

    // Create model
    Model model(env);

    // Add variables
    const auto& x = model.add_vars(Dim<1>(boost::num_edges(g)), 0, 1, idol::VarType::Binary, 0., "x");

    // Create map from boost edges to IDOL var
    using EdgeKey = std::pair<Vertex, Vertex>;
    std::map<EdgeKey, Var> edge_to_var;
    auto [it_edges, it_edges_end] = boost::edges(g);
    for (unsigned int i = 0; it_edges != it_edges_end; ++it_edges, ++i) {
        edge_to_var.emplace(EdgeKey{boost::source(*it_edges, g), boost::target(*it_edges, g)}, x[i]);
    }

    // Create map from IDOL var to boost edges
    std::map<Var, EdgeKey> var_to_edge;
    for (const auto& [edge, var] : edge_to_var) {
        var_to_edge.emplace(var, edge);
    }

    // Add flow constraints
    for(const auto& v: boost::make_iterator_range(boost::vertices(g))){
        LinExpr expr;
        for (auto e : boost::make_iterator_range(boost::edges(g))) {
            if (boost::target(e,g) == v) {
                expr -= edge_to_var.at({boost::source(e,g), boost::target(e,g)});
            }
            if (boost::source(e,g) == v) {
                expr += edge_to_var.at({boost::source(e,g), boost::target(e,g)});
            }
        }
        if(v == s){
            model.add_ctr(expr == 1, "flow_out_" + std::to_string(v));
        }
        else if(v == t){
            model.add_ctr(expr == -1, "flow_in_" + std::to_string(v));
        }
        else{
            model.add_ctr(expr == 0, "flow_null_" + std::to_string(v));
        }
    }

    const std::vector<double> resources{4., 2., 3., 2., 2., 2.};
    const double capacity = 6.;

    // Add resource constraint
    model.add_ctr(idol_Sum(k, Range(boost::num_edges(g)), resources[k] * x[k]) <= capacity, "resource");

    // Set the objective function
    auto weight_map = get(boost::edge_weight, g);
    LinExpr obj_expr;
    for (const auto& e : boost::make_iterator_range(edges(g))) {
        obj_expr += weight_map[e] * edge_to_var.at(EdgeKey{boost::source(e, g), boost::target(e, g)});
    }
    model.set_obj_expr(obj_expr);

    std::cout << model << std::endl;

    /*****************************************************************/
    /* Annotate the model to perform the Dantzig-Wolfe decomposition */
    /*****************************************************************/

    // Create decomposition annotation
    const Annotation decomposition(env, "decomposition", MasterId); // By default, everything remains in the master problem

    // Annotate constraints to be moved to the subproblems
    for(const auto& ctr : model.ctrs()){
        if(ctr.name().rfind("flow_", 0) == 0){
            ctr.set(decomposition, 0);
        }
    }

    /*****************************************/
    /* Build the column generation algorithm */
    /*****************************************/

    // Create the Dantzig-Wolfe decomposition algorithm
    auto column_generation = DantzigWolfeDecomposition(decomposition);

    // Use GLPK to solve the master problem
    column_generation.with_master_optimizer(GLPK::ContinuousRelaxation());

    // Define lambda function for solving subproblems
    const auto lambda = [&g, &s, &t, &edge_to_var, &var_to_edge](LambdaContext& t_ctx){

        // get objective function from model
        const auto& obj_func = t_ctx.get_model().get_obj_expr().affine().linear();

        // update weight map from objective function
        auto weight_map = get(boost::edge_weight, g);
        for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
            const auto& var = edge_to_var.at(EdgeKey{boost::source(edge, g), boost::target(edge, g)});
            boost::put(boost::edge_weight, g, edge, obj_func.get(var));
        }

        // solve subproblem using Dijkstra shortest path
        std::vector<double> dist(num_vertices(g));
        std::vector<Vertex> pred(num_vertices(g));
        dijkstra_shortest_paths(
            g,
            s,
            boost::weight_map(weight_map)
            .predecessor_map(&pred[0])
            .distance_map(&dist[0])
        );

        // build solution path
        std::vector<Vertex> path;
        for (Vertex v = t; v != s; v = pred[v]) {
            path.push_back(v);
        }
        path.push_back(s);
        std::ranges::reverse(path);

        // set solution to model
        for (unsigned int i = 0; i+1 < path.size(); ++i) {
            auto [e, found] = boost::edge(path[i], path[i+1], g);
            if (found) {
                auto const& var = edge_to_var.at(EdgeKey{boost::source(e, g), boost::target(e, g)});
                t_ctx.set_var_primal(var, 1.);
            }
        }

        t_ctx.set_status(Optimal);
        const auto best_obj = dist[t] + t_ctx.get_model().get_obj_expr().affine().constant();
        t_ctx.set_best_obj(best_obj);
        t_ctx.set_best_bound(best_obj);
    };

    // All subproblems will be solved using the lambda function
    const auto subproblem_specifications = DantzigWolfe::SubProblem().add_optimizer(LambdaOptimizer(lambda));
    column_generation.with_default_sub_problem_spec(subproblem_specifications);

    // Use the Farkas pricing strategy to generate columns when the master problem is infeasible
    column_generation.with_infeasibility_strategy(DantzigWolfe::FarkasPricing());

    // Apply branching to the master problem
    column_generation.with_hard_branching(false);

    // Turn on logs
    column_generation.with_logs(true);

    /*****************************************/
    /* Create the branch-and-bound algorithm */
    /*****************************************/

    auto branch_and_bound = BranchAndBound();

    // Use the most infeasible branching rule
    branch_and_bound.with_branching_rule(MostInfeasible());

    // Select nodes according to the best bound rule
    branch_and_bound.with_node_selection_rule(BestBound());

    // Turn on logs
    branch_and_bound.with_logs(true);

    /*****************************************/
    /* Create the branch-and-price algorithm */
    /*****************************************/
    const auto branch_and_price = branch_and_bound + column_generation;

    /************************************/
    /* Solve the model by decomposition */
    /************************************/
    model.use(branch_and_price);
    model.optimize();

    // Print solution
    print_solution_status(model);

    return 0;
}