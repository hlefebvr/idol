//
// Created by henri on 14/12/22.
//

#ifndef IDOL_DANTZIGWOLFESP_H
#define IDOL_DANTZIGWOLFESP_H

#include <memory>
#include "../Algorithm.h"
#include "Pool.h"

class DantzigWolfe;

class DantzigWolfeSP {
    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    DantzigWolfe& m_parent;
    unsigned int m_index;
    TempVar m_column_template;

    std::unique_ptr<Algorithm> m_exact_solution_strategy;

    Pool<Var> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    Model& model();
public:
    DantzigWolfeSP(DantzigWolfe& t_parent, unsigned int t_index);

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&& ...t_args);

    void initialize();
    void update();
    void solve();
    bool can_enrich_master();
    void enrich_master_problem();
    SolutionStatus status() const;

    PresentGenerators present_generators() const { return m_present_generators; }

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }
    const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    void contribute_to_primal_solution(Solution::Primal &t_primal) const;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &DantzigWolfeSP::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(model(), std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(result);
    return *result;
}


#endif //IDOL_DANTZIGWOLFESP_H
