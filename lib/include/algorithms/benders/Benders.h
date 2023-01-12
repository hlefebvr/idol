//
// Created by henri on 04/01/23.
//

#ifndef IDOL_BENDERS_H
#define IDOL_BENDERS_H

#include "reformulations/Reformulations_Benders.h"
#include "../Algorithm.h"
#include "BendersSP.h"
#include "Callbacks_Benders.h"

class Benders : public Algorithm {
    friend class Callbacks::Benders;

    Model& m_model;
    std::vector<BendersSP> m_subproblems;
    std::unique_ptr<Algorithm> m_master_solution_strategy;
    const bool m_is_nested;

    double m_best_lower_bound = -Inf;
    double m_best_upper_bound = +Inf;
    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;
    bool m_current_subproblems_are_all_feasible = true;

    std::optional<Solution::Primal> m_current_primal_solution;
protected:
    void execute() override;

    void initialize();

    void main_loop(bool t_solve_master = false);

    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;

public:
    explicit Benders(Model& t_model, bool t_is_nested = true);

    Model& master_problem() { return m_model; }
    [[nodiscard]] const Model& master_problem() const { return m_model; }

    BendersSP& subproblem(unsigned int t_index) { return m_subproblems[t_index-1]; }
    [[nodiscard]] const BendersSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index-1]; }

    IteratorForward<std::vector<BendersSP>> subproblems() { return m_subproblems; }
    [[nodiscard]] ConstIteratorForward<std::vector<BendersSP>> subproblems() const { return m_subproblems; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_master_solution_strategy(ArgsT&& ...t_args);

    BendersSP& add_subproblem(Model& t_model, const Ctr& t_cut, const Var& t_epigraph);

    Algorithm& master_solution_strategy() { return *m_master_solution_strategy; }
    [[nodiscard]] const Algorithm& master_solution_strategy() const { return *m_master_solution_strategy; }

    [[nodiscard]] Solution::Primal primal_solution() const override;

    void solve_master_problem();

    void analyze_master_problem_solution();

    void log_master_solution(bool t_force = false) const;

    [[nodiscard]] bool stopping_condition() const;

    void update_subproblems();

    void solve_subproblems();

    void log_subproblems_solution() const;

    void log_subproblem_solution(const BendersSP& t_subproblem, bool t_force = false) const;

    void analyze_subproblems_solution();

    void clean_up();

    void enrich_master_problem();

    using Algorithm::set;
    using Algorithm::get;

    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &Benders::set_master_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(m_model, std::forward<ArgsT>(t_args)...);
    if (m_is_nested) {
        result->template set_user_callback<Callbacks::Benders>(*this);
    }
    m_master_solution_strategy.reset(result);
    return *result;
}

#endif //IDOL_BENDERS_H
