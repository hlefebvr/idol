//
// Created by henri on 12/12/22.
//

#ifndef IDOL_GENERATION_H
#define IDOL_GENERATION_H

#include <vector>
#include "algorithms/decomposition/GenerationAlgorithm.h"

template<class SubProblemT>
class Generation : public GenerationAlgorithm {
    std::vector<SubProblemT> m_subproblems;
protected:
    virtual void initialize();
    virtual void solve_master_problem();
    virtual void analyze_master_problem_solution() = 0;
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution() = 0;
    virtual void enrich_master_problem();

    void execute() override;
public:
    explicit Generation(Algorithm& t_rmp_solution_strategy);

    void reserve(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    using ConstSubProblems = ConstIteratorForward<std::vector<SubProblemT>>;
    using SubProblems = IteratorForward<std::vector<SubProblemT>>;

    SubProblems subproblems() { return m_subproblems; }
    ConstSubProblems subproblems() const { return m_subproblems; }

    SubProblemT& subproblem(unsigned int t_index) { return m_subproblems.at(t_index); }
    const SubProblemT& subproblem(unsigned int t_index) const { return m_subproblems.at(t_index); }

    template<class ...ArgsT> SubProblemT& add_subproblem(ArgsT... t_args);
};

template<class SubProblemT>
Generation<SubProblemT>::Generation(Algorithm &t_rmp_solution_strategy) : GenerationAlgorithm(t_rmp_solution_strategy) {

}

template<class SubProblemT>
void Generation<SubProblemT>::initialize() {

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

template<class SubProblemT>
void Generation<SubProblemT>::execute() {

    initialize();

    while (true) {

        solve_master_problem();

        analyze_master_problem_solution();

        if (is_terminated()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        if (is_terminated()) { break; }

        enrich_master_problem();

    }

}

template<class SubProblemT>
void Generation<SubProblemT>::solve_master_problem() {
    rmp_solution_strategy().solve();
}

template<class SubProblemT>
void Generation<SubProblemT>::update_subproblems() {

    for (auto& subproblem : m_subproblems) {
        subproblem.update();
    }

}

template<class SubProblemT>
void Generation<SubProblemT>::solve_subproblems() {

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        if (is_terminated()) { break; }
    }

}

template<class SubProblemT>
void Generation<SubProblemT>::enrich_master_problem() {

    bool master_has_been_enriched = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.can_enrich_master()) {
            subproblem.enrich_master_problem();
            master_has_been_enriched = true;
        }

    }

    if (!master_has_been_enriched) {
        set_reason(Proved);
        terminate();
    }

}

template<class SubProblemT>
template<class... ArgsT>
SubProblemT &Generation<SubProblemT>::add_subproblem(ArgsT... t_args) {
    m_subproblems.emplace_back(*this, std::forward<ArgsT>(t_args)...);
    return m_subproblems.back();
}

template<class SubProblemT>
void Generation<SubProblemT>::reserve(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
}

#endif //IDOL_GENERATION_H
