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

    std::cout << model << std::endl;
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

void direct_solve(const Model& model){

    std::cout <<  "Direct GLPK solving" << std::endl;

    Model direct_model(model.copy());

    direct_model.use(GLPK());
    direct_model.optimize();
    
    print_solution_status(direct_model);
}

void branch_and_price_solve(Env& env, const Model& model){

    std::cout <<  "Branch-and-Price GLPK + GLPK solving" << std::endl;

    Model bap_model(model.copy());

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

void direct_solve_using_lambda_function(const Model& model){

    std::cout <<  "Direct lambda function solving" << std::endl;

    Model direct_model(model.copy());

    const auto lambda = [](LambdaContext& t_ctx){
        std::cout << "Begin lambda" << std::endl; 

        auto copy = t_ctx.get_model().copy();
        copy.use(GLPK());
        copy.optimize();
        print_solution_status(copy); 

        t_ctx.set_status(Optimal);
        t_ctx.set_best_obj(copy.get_best_obj());
        t_ctx.set_best_bound(copy.get_best_bound());

        for (const auto& var : copy.vars()) {
            t_ctx.set_var_primal(var, copy.get_var_primal(var));
        }
        std::cout <<  "End lambda" << std::endl; 
    };

    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    direct_model.use(LambdaOptimizer(lambda));
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    direct_model.optimize();
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    
}

void branch_and_price_solve_using_lambda_function(Env& env, const Model& model){

    std::cout <<  "Branch-and-Price GLPK + Lambda solving" << std::endl;

    Model bap_model(model.copy());

    const Annotation decomposition(env, "decomposition", MasterId);

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

    const auto lambda = [](LambdaContext& t_ctx){
        std::cout << "Begin lambda" << std::endl; 

        auto copy = t_ctx.get_model().copy();
        std::cout << "vars = " << copy.vars().size() << std::endl;
        std::cout << "cons = " << copy.ctrs().size() << std::endl;

        copy.use(GLPK());
        copy.optimize();
        print_solution_status(copy); 

        t_ctx.set_status(Optimal);
        t_ctx.set_best_obj(copy.get_best_obj());
        t_ctx.set_best_bound(copy.get_best_bound());

        for (const auto& var : copy.vars()) {
            t_ctx.set_var_primal(var, copy.get_var_primal(var));
        }

        std::cout <<  "End lambda" << std::endl; 
    };

    const auto subproblem_specifications = DantzigWolfe::SubProblem().add_optimizer(LambdaOptimizer(lambda));
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

void branch_and_price_solve_using_lambda_function_using_boost(){

    std::cout <<  "Branch-and-Price GLPK + Lambda solving" << std::endl;

    // Create boost graph

    using Graph = boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            boost::no_property,
            boost::property<boost::edge_weight_t, double>
        >;

    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
    using Edge = boost::graph_traits<Graph>::edge_descriptor;

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

    // Create IDOL model

    Env env;
    Model model(env);

    const auto& x = model.add_vars(Dim<1>(boost::num_edges(g)), 0, 1, idol::VarType::Binary, 0., "x");

    using EdgeKey = std::pair<Vertex, Vertex>;
    std::map<EdgeKey, Var> edge_to_var;
    auto [it_edges, it_edges_end] = boost::edges(g);
    for (unsigned int i = 0; it_edges != it_edges_end; ++it_edges, ++i) {
        edge_to_var.emplace(EdgeKey{boost::source(*it_edges, g), boost::target(*it_edges, g)}, x[i]);
    }

    std::map<Var, EdgeKey> var_to_edge;
    for (const auto& [edge, var] : edge_to_var) {
        var_to_edge.emplace(var, edge);
    }


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

    model.add_ctr(idol_Sum(k, Range(boost::num_edges(g)), resources[k] * x[k]) <= capacity, "resource");

    auto weight_map = get(boost::edge_weight, g);
    QuadExpr obj_expr;
    for (const auto& e : boost::make_iterator_range(edges(g))) {
        obj_expr += weight_map[e] * edge_to_var.at(EdgeKey{boost::source(e, g), boost::target(e, g)});
    }

    model.set_obj_expr(obj_expr);

    std::cout << model << std::endl;

    const Annotation decomposition(env, "decomposition", MasterId);

    for(const auto& ctr : model.ctrs()){
        if(ctr.name().rfind("flow_", 0) == 0){
            ctr.set(decomposition, 0);
        }
    }

    auto column_generation = DantzigWolfeDecomposition(decomposition);
    column_generation.with_master_optimizer(GLPK::ContinuousRelaxation());

    const auto lambda = [&g, &s, &t, &edge_to_var, &var_to_edge](LambdaContext& t_ctx){
        std::cout << "Begin lambda" << std::endl;

        std::vector<double> dist(num_vertices(g));
        std::vector<Vertex> pred(num_vertices(g));

        const auto& obj_func = t_ctx.get_model().get_obj_expr().affine().linear();

        auto weight_map = get(boost::edge_weight, g);
        for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
            const auto& var = edge_to_var.at(EdgeKey{boost::source(edge, g), boost::target(edge, g)});
            boost::put(boost::edge_weight, g, edge, obj_func.get(var));
        }

        //dump
        for (auto e : boost::make_iterator_range(boost::edges(g))) {

            auto u = boost::source(e, g);
            auto v = boost::target(e, g);

            auto var = edge_to_var.at(EdgeKey{boost::source(e, g), boost::target(e, g)});
            auto w = boost::get(boost::edge_weight, g, e);

            std::cout
                << u << " -> " << v
                << " | var=" << var
                << " | cost=" << w
                << "\n";
        }

        dijkstra_shortest_paths(
            g,
            s,
            boost::weight_map(weight_map)
            .predecessor_map(&pred[0])
            .distance_map(&dist[0])
        );

        std::cout << "Distance s -> t = " << dist[t] << std::endl;

        std::vector<Vertex> path;
        for (Vertex v = t; v != s; v = pred[v]) {
            path.push_back(v);
        }
        path.push_back(s);
        std::ranges::reverse(path);

        std::cout << "Path : " << std::endl;

        for (unsigned int i = 0; i+1 < path.size(); ++i) {
            auto [e, found] = boost::edge(path[i], path[i+1], g);
            if (found) {
                auto const& var = edge_to_var.at(EdgeKey{boost::source(e, g), boost::target(e, g)});
                t_ctx.set_var_primal(var, 1.);
                std::cout << var << std::endl;
            }
        }
        std::cout << std::endl;

        t_ctx.set_status(Optimal);
        const auto best_obj = dist[t] + t_ctx.get_model().get_obj_expr().affine().constant();
        t_ctx.set_best_obj(best_obj);
        t_ctx.set_best_bound(best_obj);

        std::cout <<  "End lambda" << std::endl;
    };

    const auto subproblem_specifications = DantzigWolfe::SubProblem().add_optimizer(LambdaOptimizer(lambda));
    column_generation.with_default_sub_problem_spec(subproblem_specifications);
    column_generation.with_infeasibility_strategy(DantzigWolfe::FarkasPricing());
    column_generation.with_hard_branching(false);
    column_generation.with_logs(true);

    auto branch_and_bound = BranchAndBound();
    branch_and_bound.with_branching_rule(MostInfeasible());
    branch_and_bound.with_node_selection_rule(BestBound());
    branch_and_bound.with_logs(true);

    const auto branch_and_price = branch_and_bound + column_generation;

    model.use(branch_and_price);

    model.optimize();

    print_solution_status(model);
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
    else if (mode == "bap_lambda"){
        branch_and_price_solve_using_lambda_function(env, model);
    }
    else if (mode == "bap_lambda_boost"){
        branch_and_price_solve_using_lambda_function_using_boost();
    }
    
    
    return 0;
}
