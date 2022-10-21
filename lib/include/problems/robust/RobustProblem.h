//
// Created by henri on 21/10/22.
//

#ifndef IDOL_ROBUSTPROBLEM_H
#define IDOL_ROBUSTPROBLEM_H

#include "../../modeling/models/Model.h"

namespace Problems {
    template<unsigned int N_STAGES> class Robust;
}

namespace impl::Problems::Robust {
    class StageChecker;
}

class impl::Problems::Robust::StageChecker : public ::Listener {
public:
};

template<unsigned int N_STAGES = 1>
class Problems::Robust {
    static_assert(N_STAGES >= 1);

    std::array<Model, N_STAGES> m_models;
    std::array<Model, N_STAGES> m_uncertainty_sets;
    std::array<impl::Problems::Robust::StageChecker, N_STAGES> m_model_stage_checkers;
    std::array<impl::Problems::Robust::StageChecker, N_STAGES> m_uncertainty_set_stage_checkers;
public:
    Robust();

    Model& stage(unsigned int t_stage) { return m_models[t_stage]; }
    Model& uncertainty_set(unsigned int t_stage = 0) { return m_uncertainty_sets[t_stage]; }
};

template<unsigned int N_STAGES>
Problems::Robust<N_STAGES>::Robust() {

}

#endif //IDOL_ROBUSTPROBLEM_H
