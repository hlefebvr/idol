//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_COLGENMODEL_H
#define OPTIMIZE_COLGENMODEL_H

#include <vector>
#include "SubProblem.h"

class Model;

class ColGenModel {
    Model& m_rmp;
    std::vector<SubProblem> m_sub_problems;
public:
    explicit ColGenModel(Model& t_rmp);

    SubProblem::Id create_sub_problem(Model& t_model);
};

#endif //OPTIMIZE_COLGENMODEL_H
