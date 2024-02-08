//
// Created by henri on 08.02.24.
//
#include "idol/optimizers/column-and-constraint-generation/separators/MaxMinBiLevel.h"

idol::ColumnAndConstraintGenerationSeparator *idol::ColumnAndConstraintGenerationSeparators::MaxMinBiLevel::clone() const {
    return new MaxMinBiLevel(*this);
}

