//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATIONALGORITHM_H
#define OPTIMIZE_GENERATIONALGORITHM_H

#include "../Algorithm.h"

class GenerationAlgorithm {
    Algorithm& m_rmp_solution_strategy;
protected:
    Algorithm& rmp_solution_strategy();
    [[nodiscard]] const Algorithm& rmp_solution_strategy() const;
public:
    explicit GenerationAlgorithm(Algorithm& t_decomposition_id);
};

template<class ...AttrT>
class GenerationAlgorithmWithAttributes : public AlgorithmWithAttributes<AttrT...>, public GenerationAlgorithm {
public:
    explicit GenerationAlgorithmWithAttributes(Algorithm& t_rmp_solution_strategy);
};

template<class ...AttrT>
GenerationAlgorithmWithAttributes<AttrT...>::GenerationAlgorithmWithAttributes(Algorithm& t_rmp_solution_strategy)
        : GenerationAlgorithm(t_rmp_solution_strategy) {

}

#endif //OPTIMIZE_GENERATIONALGORITHM_H
