//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATIONALGORITHM_H
#define OPTIMIZE_GENERATIONALGORITHM_H

#include "../Algorithm.h"
#include "../decomposition/DecompositionId.h"

class GenerationAlgorithm {
    DecompositionId m_id;
protected:
    Algorithm& rmp_solution_strategy();
    [[nodiscard]] const Algorithm& rmp_solution_strategy() const;
public:
    GenerationAlgorithm(DecompositionId&& t_decomposition_id);
};

template<class ...AttrT>
class GenerationAlgorithmWithAttributes : public AlgorithmWithAttributes<AttrT...>, public GenerationAlgorithm {
public:
    explicit GenerationAlgorithmWithAttributes(DecompositionId&& t_decomposition_id);
};

template<class ...AttrT>
GenerationAlgorithmWithAttributes<AttrT...>::GenerationAlgorithmWithAttributes(DecompositionId &&t_decomposition_id)
        : GenerationAlgorithm(std::move(t_decomposition_id)) {

}

#endif //OPTIMIZE_GENERATIONALGORITHM_H
