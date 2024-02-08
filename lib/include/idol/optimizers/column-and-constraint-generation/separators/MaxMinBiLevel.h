//
// Created by henri on 08.02.24.
//

#ifndef IDOL_MAXMINBILEVEL_H
#define IDOL_MAXMINBILEVEL_H

#include "idol/optimizers/column-and-constraint-generation/ColumnAndConstraintGenerationSeparator.h"

namespace idol::ColumnAndConstraintGenerationSeparators {
    class MaxMinBiLevel;
}

class idol::ColumnAndConstraintGenerationSeparators::MaxMinBiLevel : public idol::ColumnAndConstraintGenerationSeparator {
public:
    ColumnAndConstraintGenerationSeparator *clone() const override;
};

#endif //IDOL_MAXMINBILEVEL_H
