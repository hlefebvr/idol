//
// Created by henri on 06.03.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/NoStabilization.h"

idol::Robust::CCGStabilizer::Strategy *
idol::Robust::CCGStabilizers::NoStabilization::operator()() const {
    return nullptr;
}

idol::Robust::CCGStabilizer *idol::Robust::CCGStabilizers::NoStabilization::clone() const {
    return new NoStabilization(*this);
}

