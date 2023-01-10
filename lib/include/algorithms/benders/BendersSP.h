//
// Created by henri on 04/01/23.
//

#ifndef IDOL_BENDERSSP_H
#define IDOL_BENDERSSP_H

#include "../Algorithm.h"
#include "../dantzig-wolfe/Pool.h"

class Benders;

class BendersSP {
    using PresentGeneratorsList = std::list<std::pair<Ctr, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    Benders& m_parent;
    Model& m_model;
    unsigned int m_index;
    const Var& m_epigraph_variable;
    TempCtr m_cut_template;

    std::unique_ptr<Algorithm> m_exact_solution_strategy;

    Pool<Ctr> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    Model& model();
public:
    BendersSP(Benders& t_parent, unsigned int t_index, Model& t_model, const Ctr& t_cut, const Var& t_epigraph);

    void initialize();

    void update(const Solution::Primal& t_master_solution);

    void solve();

    void enrich_master_problem();

    [[nodiscard]] SolutionStatus status() const;

    [[nodiscard]] double objective_value() const;

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&& ...t_args);

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }
    [[nodiscard]] const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    [[nodiscard]] const Var& epigraph_variable() const { return m_epigraph_variable; }

    [[nodiscard]] TempCtr create_cut_from_generator(const Solution::Primal& t_generator) const;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &BendersSP::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(model(), std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(result);
    return *result;
}

#endif //IDOL_BENDERSSP_H
