//
// Created by henri on 30.10.23.
//
#include "idol/optimizers/mixed-integer-programming/wrappers/MinKnap/MinKnap.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/MinKnap/Optimizers_MinKnap.h"

idol::Optimizer *idol::MinKnap::operator()(const Model &t_model) const {
#ifdef IDOL_USE_MINKNAP
    return new Optimizers::MinKnap(t_model);
#else
    throw Exception("idol was not linked to minknap.");
#endif
}

idol::MinKnap *idol::MinKnap::clone() const {
    return new MinKnap(*this);
}

