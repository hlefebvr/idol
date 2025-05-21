//
// Created by henri on 07.05.25.
//

#ifndef IDOL_OPTIMIZERS_COLUNA_H
#define IDOL_OPTIMIZERS_COLUNA_H

#include "Optimizers_JuMP.h"

#ifdef IDOL_USE_JULIA

namespace idol::Optimizers {
    class Coluna;
}

class idol::Optimizers::Coluna : public Optimizers::JuMP {
    const Annotation<unsigned int> m_annotation;
public:
    Coluna(const Model& t_parent, const Annotation<unsigned int>& t_annotation);

protected:
    uint64_t hook_create_julia_model(jl_value_t* t_optimizer) override;

    void hook_optimize() override;
};

#endif // IDOL_USE_JULIA

#endif //IDOL_OPTIMIZERS_COLUNA_H
