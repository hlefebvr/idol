//
// Created by henri on 04/01/23.
//

#ifndef IDOL_BENDERSSP_H
#define IDOL_BENDERSSP_H

#include "../Algorithm.h"
#include "../dantzig-wolfe/Pool.h"

class Benders;

class BendersSP {
    Benders& m_parent;
    unsigned int m_index;
    TempCtr m_cut_template;

    Ctr m_bounded_theta;

    std::unique_ptr<Algorithm> m_exact_solution_strategy;
protected:
    Model& model();

    void remove_fixed_theta_to_zero();
public:
    BendersSP(Benders& t_parent, unsigned int t_index);

    void initialize();

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&& ...t_args);
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &BendersSP::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(model(), std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(result);
    return *result;
}

#endif //IDOL_BENDERSSP_H
