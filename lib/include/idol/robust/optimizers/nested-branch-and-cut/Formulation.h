//
// Created by Henri on 03/02/2026.
//

#ifndef IDOL_ROBUST_NESTEDBRANCHANDCUT_FORMULATION_H
#define IDOL_ROBUST_NESTEDBRANCHANDCUT_FORMULATION_H
#include "idol/bilevel/modeling/Description.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Robust::NBC {
    class Formulation;
}

namespace idol::Optimizers::Robust {
    class NestedBranchAndCut;
}

class idol::Robust::NBC::Formulation {
    const idol::Optimizers::Robust::NestedBranchAndCut& m_parent;

    Model m_model;
    std::vector<Var> m_first_stage_decisions;
    idol::Bilevel::Description m_bilevel_description;
protected:
    void build_model();
public:
    Formulation(const idol::Optimizers::Robust::NestedBranchAndCut& t_parent);

    Model& model() { return m_model; }
    const Model& model() const { return m_model; }

    const idol::Bilevel::Description& bilevel_description() const { return m_bilevel_description; }

    auto first_stage_decisions()const { return ConstIteratorForward(m_first_stage_decisions); }
};

#endif //IDOL_ROBUST_NESTEDBRANCHANDCUT_FORMULATION_H