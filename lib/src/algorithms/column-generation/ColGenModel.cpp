//
// Created by henri on 12/09/22.
//
#include "algorithms/column-generation/ColGenModel.h"

ColGenModel::ColGenModel(Model& t_rmp) : m_rmp(t_rmp) {

}

SubProblem::Id ColGenModel::create_sub_problem(Model &t_model) {
    return SubProblem::Id();
}
