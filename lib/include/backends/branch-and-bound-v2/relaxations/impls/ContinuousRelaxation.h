//
// Created by henri on 21/03/23.
//

#ifndef IDOL_CONTINUOUSRELAXATION_H
#define IDOL_CONTINUOUSRELAXATION_H

#include "backends/branch-and-bound-v2/relaxations/factories/RelaxationBuilderFactory.h"
#include "modeling/models/AbstractModel.h"

class ContinuousRelaxation : public RelaxationBuilderFactory {
public:
    AbstractModel *operator()(const AbstractModel &t_model) const override {

        auto* result = t_model.clone();

        for (const auto& var : t_model.vars()) {
            if (t_model.get(Attr::Var::Type, var) != Continuous) {
                result->set(Attr::Var::Type, var, Continuous);
            }
        }

        return result;
    }

    [[nodiscard]] ContinuousRelaxation *clone() const override {
        return new ContinuousRelaxation(*this);
    }
};

#endif //IDOL_CONTINUOUSRELAXATION_H
