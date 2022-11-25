//
// Created by henri on 17/10/22.
//

#ifndef IDOL_ATTRIBUTES_BRANCHANDBOUND_H
#define IDOL_ATTRIBUTES_BRANCHANDBOUND_H

#include "../parameters/Parameters.h"

IDOL_CREATE_PARAMETER_CLASS(BranchAndBound)

IDOL_CREATE_PARAMETER_TYPE(BranchAndBound, int, 1)
IDOL_CREATE_PARAMETER(BranchAndBound, int, 0, NodeSelection, 0)

enum NodeSelections : int {
    Automatic,
    WorstBound,
    BestBound,
    DepthFirst,
    BreadthFirst
};

#endif //IDOL_ATTRIBUTES_BRANCHANDBOUND_H
