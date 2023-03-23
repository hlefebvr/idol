//
// Created by henri on 03/02/23.
//

#ifndef IDOL_RELAXATION_H
#define IDOL_RELAXATION_H

#include "modeling/models/AbstractModel.h"

class Relaxation {
public:
    virtual ~Relaxation() = default;

    virtual AbstractModel& model() = 0;

    [[nodiscard]] virtual const AbstractModel& model() const = 0;

    virtual void build() = 0;
};

#endif //IDOL_RELAXATION_H
