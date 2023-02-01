//
// Created by henri on 14/12/22.
//

#ifndef IDOL_DANTZIGWOLFESP_H
#define IDOL_DANTZIGWOLFESP_H

#include <memory>
#include "../Algorithm.h"
#include "Pool.h"
#include "BranchingManager.h"

class DantzigWolfe;

class DantzigWolfeSP {
    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    DantzigWolfe& m_parent;
    unsigned int m_index;
    TempVar m_column_template;

    std::unique_ptr<Algorithm> m_exact_solution_strategy;
    std::unique_ptr<BranchingManager> m_branching_manager;

    Pool<Var> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    Model& model();
    [[nodiscard]] Solution::Primal in_original_space(const Solution::Primal& t_primals) const;
    void remove_column_if(const std::function<bool(const Var&, const Solution::Primal&)>& t_indicator_for_removal);
    void restore_column_from_pool();
public:
    DantzigWolfeSP(DantzigWolfe& t_parent, unsigned int t_index);

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&& ...t_args);

    template<class ManagerT, class ...ArgsT> ManagerT& set_branching_manager(ArgsT&& ...t_args);

    void initialize();
    void update(bool t_farkas_pricing, const Solution::Dual& t_duals);
    void solve();
    [[nodiscard]] double objective_value() const;
    [[nodiscard]] double reduced_cost(const Solution::Dual& t_duals) const;
    void enrich_master_problem();
    void clean_up();
    [[nodiscard]] SolutionStatus status() const;

    [[nodiscard]] PresentGenerators present_generators() const { return m_present_generators; }

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }
    [[nodiscard]] const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    void contribute_to_primal_solution(Solution::Primal &t_primal) const;

    void apply_bound_expressed_in_original_space(const Req<double, Var>& t_attr, const Var& t_var, double t_value);

    [[nodiscard]] TempVar create_column_from_generator(const Solution::Primal& t_generator) const;

    TempVar& column_template() { return m_column_template; }
    [[nodiscard]] const TempVar& column_template() const { return m_column_template; }

    DantzigWolfe& parent() { return m_parent; }
    [[nodiscard]] const DantzigWolfe& parent() const { return m_parent; }

    [[nodiscard]] LinExpr<Var> expand_variable(const Var& t_var) const;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &DantzigWolfeSP::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(model(), std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(result);
    return *result;
}

template<class ManagerT, class... ArgsT>
ManagerT &DantzigWolfeSP::set_branching_manager(ArgsT &&... t_args) {
    auto* result = new ManagerT(*this, std::forward<ArgsT>(t_args)...);
    m_branching_manager.reset(result);
    return *result;
}


#endif //IDOL_DANTZIGWOLFESP_H
