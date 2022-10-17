//
// Created by henri on 17/10/22.
//

#ifndef IDOL_ATTRIBUTES_BRANCHANDBOUND_H
#define IDOL_ATTRIBUTES_BRANCHANDBOUND_H


#include "../attributes/AttributesMacros.h"

CREATE_ATTRIBUTE_SECTION(BranchAndBound, 1);
CREATE_ATTRIBUTE(BranchAndBound, 0, unsigned int, NodeSelection, 0);

namespace NodeSelections {
    static constexpr unsigned int Automatic = 0;
    static constexpr unsigned int WorstBound = 1;
    static constexpr unsigned int BestBound = 2;
    static constexpr unsigned int DepthFirst = 3;
    static constexpr unsigned int BreadthFirst = 4;
}

#endif //IDOL_ATTRIBUTES_BRANCHANDBOUND_H
