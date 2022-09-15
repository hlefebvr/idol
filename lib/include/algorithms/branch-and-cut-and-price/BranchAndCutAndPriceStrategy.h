//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_BRANCHANDCUTANDPRICESTRATEGY_H
#define OPTIMIZE_BRANCHANDCUTANDPRICESTRATEGY_H

#include "../branch-and-bound/SolutionStrategy.h"
#include "../../containers/Map.h"

class SubProblem {

};

template<class SolverRMPT>
class BranchAndCutAndPriceStrategy : public SolutionStrategy<SolverRMPT> {
    Map<unsigned int, SubProblem> m_sub_problems;
public:
    explicit BranchAndCutAndPriceStrategy(Model& t_model) : SolutionStrategy<SolverRMPT>(t_model) {}
};

#endif //OPTIMIZE_BRANCHANDCUTANDPRICESTRATEGY_H
