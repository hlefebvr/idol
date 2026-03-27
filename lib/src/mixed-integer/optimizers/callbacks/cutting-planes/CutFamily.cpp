//
// Created by Henri on 26/03/2026.
//
#include <utility>

#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CutFamily.h"

namespace idol {
    class TempCtr;
}

idol::CutFamily::CutFamily(CglCutGenerator* t_cgl_generator, std::string  t_name) :
#ifdef IDOL_USE_CGL
    m_cgl_generator(t_cgl_generator),
#endif
    m_name(std::move(t_name)) {

}

double idol::CutFamily::score() const {

    if (m_n_generated == 0) {
        return 0.0;
    }

    return (
        .9 * average_effectiveness()
      + .05 * maximum_effectiveness()
      + .05 * acceptance_ratio()
      ) / m_timer.cumulative_count();

}

void idol::CutFamily::add_effectiveness_statistics(double t_effectiveness) {

    m_sum_effectiveness += t_effectiveness;

    if (t_effectiveness > m_max_effectiveness) {
        m_max_effectiveness = t_effectiveness;
    }

}

OsiCuts idol::CutFamily::generate(OsiSolverInterface& t_osi_solver, int t_agressiveness) {
#ifdef IDOL_USE_CGL
    m_cgl_generator->setAggressiveness(t_agressiveness);

    OsiCuts result;
    m_timer.start();
    m_cgl_generator->generateCuts(t_osi_solver, result);
    m_timer.stop();

    m_n_generated += result.sizeRowCuts();
    m_n_calls++;

    return result;
#else
    return OsiCuts();
#endif
}
