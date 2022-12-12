#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"

class Pipe : public Algorithm {
    std::list<std::pair<const Model*, UserAttr>> m_reformulated_variables;
    std::list<std::pair<const Model*, UserAttr>> m_reformulated_constraints;
    std::unique_ptr<Algorithm> m_algorithm;
public:
    Pipe() = default;

    void add_reformulated_var(const Model& t_model, const UserAttr& t_attr) {
        m_reformulated_variables.emplace_back(&t_model, t_attr);
    }

    void add_reformulated_ctr(const Model& t_model, const UserAttr& t_attr) {
        m_reformulated_constraints.emplace_back(&t_model, t_attr);
    }

    template<class T, class ...ArgsT> T& set_algorithm(ArgsT&& ...t_args) {
        auto* result = new T(std::forward<ArgsT>(t_args)...);
        m_algorithm.reset(result);
        return *result;
    }

    [[nodiscard]] const Var& reformulate(const Var& t_var) const {
        for (const auto& [ptr_to_model, user_attr] : m_reformulated_variables) {
            if (ptr_to_model->get(Attr::Var::Status, t_var)) {
                return ptr_to_model->get<Var>(user_attr, t_var);
            }
        }
        return t_var;
    }

    [[nodiscard]] const Ctr& reformulate(const Ctr& t_ctr) const {
        for (const auto& [ptr_to_model, user_attr] : m_reformulated_constraints) {
            if (ptr_to_model->get(Attr::Ctr::Status, t_ctr)) {
                return ptr_to_model->get<Ctr>(user_attr, t_ctr);
            }
        }
        return t_ctr;
    }

    void set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) override {
        Delegate::set(t_attr, reformulate(t_var), t_value);
    }

    void set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) override {
        Delegate::set(t_attr, reformulate(t_var), t_value);
    }

    void set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override {
        Delegate::set(t_attr, reformulate(t_var), t_value);
    }

    void
    set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override {
        Delegate::set(t_attr, reformulate(t_var), t_value);
    }

    void set(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override {
        Delegate::set(t_attr, reformulate(t_ctr), t_value);
    }

    void set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override {
        Delegate::set(t_attr, reformulate(t_ctr), t_value);
    }

    void
    set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override {
        Delegate::set(t_attr, reformulate(t_ctr), t_value);
    }

    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) override {
        Delegate::set(t_attr, reformulate(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override {
        Delegate::set(t_attr, reformulate(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, Constant &&t_value) override {
        Delegate::set(t_attr, reformulate(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var,
             Constant &&t_value) override {
        Delegate::set(t_attr, reformulate(t_ctr), reformulate(t_var), t_value);
    }
protected:
    void execute() override {
        m_algorithm->solve();
    }

    AttributeManager &attribute_delegate(const Attribute &t_attribute) override {
        return *m_algorithm;
    }

    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override {
        return *m_algorithm;
    }

    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override {
        return *m_algorithm;
    }
};

int main(int t_argc, const char** t_argv) {

    using namespace Problems::GAP;

    auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");

    const unsigned int n_knapsacks = instance.n_knapsacks();
    const unsigned int n_items = instance.n_items();

    Model model;
    auto complicating_constraint = model.add_user_attr<unsigned int>(0, "complicating_constraint");

    // Variables
    auto x = model.add_vars(Dim<2>(n_knapsacks, n_items), 0., 1., Binary, 0., "x");

    // Objective function
    Expr objective = idol_Sum(
                i, Range(n_knapsacks),
                idol_Sum(j, Range(n_items), instance.p(i, j) * x[i][j])
            );
    model.set(Attr::Obj::Expr, objective);

    // Knapsack constraints
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto ctr = model.add_ctr( idol_Sum(j, Range(n_items), instance.w(i, j) * x[i][j]) <= instance.t(i) );
        model.set<unsigned int>(complicating_constraint, ctr, i+1);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_knapsacks), x[i][j]) == 1);
    }

    // DW reformulation
    Reformulations::DantzigWolfe result(model, complicating_constraint);

    // Branching candidates
    std::vector<Var> branching_candidates;
    branching_candidates.reserve(n_entries<Var, 2>(x));

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            branching_candidates.emplace_back(model.get<Var>(result.reformulated_variable(), x[i][j]) );
        }
    }

    Pipe pipe;
    pipe.add_reformulated_var(model, result.reformulated_variable());
    pipe.add_reformulated_ctr(model, result.reformulated_constraint());
    auto& decomposition = pipe.set_algorithm<Decomposition>();
    decomposition.set_rmp_solution_strategy<Solvers::Gurobi>(result.restricted_master_problem());
    auto& colgen = decomposition.add_generation_strategy<ColumnGeneration>();

    for (unsigned int i = 1, n = result.subproblems().size() ; i <= n ; ++i) {
        auto& sp = colgen.add_subproblem(result.alpha(i));
        sp.set_solution_strategy<Solvers::Gurobi>(result.subproblem(i));
        sp.set_branching_scheme<ColumnGenerationBranchingSchemes::RMP>();
    }

    Log::set_level(Debug);

    pipe.solve();

    pipe.set(Attr::Var::Lb, x[0][0], 1);

    /*
    Log::set_level(Debug);

    // Branch and price
    auto solver = branch_and_price(
            result.restricted_master_problem(),
            result.alphas().begin(),
            result.alphas().end(),
            result.subproblems().begin(),
            result.subproblems().end(),
            branching_candidates
            );

    solver.solve();

    std::cout << "Optimum: " << solver.primal_solution().objective_value() << std::endl;
    */

    return 0;
}
