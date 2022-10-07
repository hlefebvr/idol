//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONSUBPROBLEM_H
#define OPTIMIZE_CUTGENERATIONSUBPROBLEM_H

#include "algorithms/Algorithm.h"
#include "containers/Set.h"
#include "CutGenerationOriginalSpaceBuilder.h"
#include <memory>
#include <list>

class CutGenerationSubproblem {

    TempCtr m_cut_template;
    Row m_objective_template;

    Algorithm& m_rmp_strategy;
    std::unique_ptr<Algorithm> m_exact_solution_strategy;
    std::unique_ptr<CutGenerationOriginalSpaceBuilder> m_original_space_builder;
    std::list<std::unique_ptr<Solution::Primal>> m_primal_solutions;
    Set<unsigned int> m_subproblem_ids;

    using PresentCutList = std::list<std::pair<Ctr, Solution::Primal&>>;

    PresentCutList m_currently_present_cuts;

    void save_subproblem_ids();
    void remove_cut_template_from_rmp(const Ctr& t_cut);
    bool is_in_subproblem(const Var& t_var) const;

    void remove_cuts_violating_lower_bound(const Var& t_var, double t_lb);

    void remove_cuts_violating_upper_bound(const Var& t_var, double t_ub);
public:
    explicit CutGenerationSubproblem(Algorithm& t_rmp_strategy, const Ctr& t_cut);

    void build();

    void solve();

    using PresentCuts = ConstIteratorForward<PresentCutList>;

    [[nodiscard]] PresentCuts currently_present_cuts() const { return PresentCuts(m_currently_present_cuts); }

    [[nodiscard]] Row get_separation_objective(const Solution::Primal &t_primals) const;

    void update_separation_objective(const Row &t_objective);

    void save_last_primal_solution();

    void log_last_primal_solution();

    bool violated_cut_found();

    void add_cut_to_rmp();

    [[nodiscard]] bool is_unbounded() const;

    [[nodiscard]] bool is_infeasible() const;

    [[nodiscard]] bool could_not_be_solved_to_optimality() const;

    [[nodiscard]] TempCtr create_cut_from(const Solution::Primal& t_primals) const;

    [[nodiscard]] Solution::Primal primal_solution() const;

    bool set_lower_bound(const Var &t_var, double t_lb);

    bool set_upper_bound(const Var &t_var, double t_ub);

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }

    Algorithm& rmp_solution_strategy() { return m_rmp_strategy; }

    void remove_cut_if(const std::function<bool(const Ctr&, const Solution::Primal&)>& t_indicator_for_removal);

    template<class T, class ...Args> T& set_solution_strategy(Args&& ...t_args);

    template<class T, class ...Args> T& set_original_space_builder(Args&& ...t_args);
};

template<class T, class... Args>
T &CutGenerationSubproblem::set_solution_strategy(Args &&... t_args) {
    auto* exact_solution_strategy = new T(std::forward<Args>(t_args)...);
    m_exact_solution_strategy.reset(exact_solution_strategy);
    return *exact_solution_strategy;
}

template<class T, class... Args>
T &CutGenerationSubproblem::set_original_space_builder(Args &&... t_args) {
    auto* generator = new T(std::forward<Args>(t_args)...);
    m_original_space_builder.reset(generator);
    return *generator;
}

#endif //OPTIMIZE_CUTGENERATIONSUBPROBLEM_H
