//
// Created by henri on 16/09/22.
//
#include "../../../include/algorithms/decomposition/GenerationAlgorithm.h"
#include "../../../include/modeling/models/Model.h"

GenerationAlgorithm::GenerationAlgorithm(Algorithm& t_rmp_solution_strategy)
        : m_rmp_solution_strategy(t_rmp_solution_strategy) {

}

Algorithm &GenerationAlgorithm::rmp_solution_strategy() {
    return m_rmp_solution_strategy;
}

const Algorithm &GenerationAlgorithm::rmp_solution_strategy() const {
    return m_rmp_solution_strategy;
}
