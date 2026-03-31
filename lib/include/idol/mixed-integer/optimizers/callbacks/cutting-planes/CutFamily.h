//
// Created by Henri on 26/03/2026.
//

#ifndef IDOL_CUTFAMILY_H
#define IDOL_CUTFAMILY_H

#include "idol/general/optimizers/Timer.h"

#include <memory>
#include <list>

namespace idol {
    class TempCtr;
    class CutFamily;
}

#ifdef IDOL_USE_CGL
#include <CglCutGenerator.hpp>
#else
class OsiSolverInterface {};
class CglCutGenerator {};
class OsiCuts {};
#endif

class idol::CutFamily {
#ifdef IDOL_USE_CGL
    std::unique_ptr<CglCutGenerator> m_cgl_generator;
#endif
    const std::string m_name;
    unsigned int m_n_calls = 0;
    unsigned int m_n_generated = 0;
    unsigned int m_n_accepted = 0;
    double m_sum_effectiveness = 0.;
    double m_max_effectiveness = 0.;
    Timer m_timer;
public:
    CutFamily(CglCutGenerator* t_cgl_generator, std::string  t_name);

    [[nodiscard]] const std::string& name() const { return m_name; }
    [[nodiscard]] double score() const;
    [[nodiscard]] unsigned int n_generated() const { return m_n_generated; }
    [[nodiscard]] unsigned int n_accepted() const { return m_n_accepted; }
    [[nodiscard]] double average_effectiveness() const { return m_sum_effectiveness / m_n_generated; }
    [[nodiscard]] double maximum_effectiveness() const { return m_max_effectiveness; }
    [[nodiscard]] double acceptance_ratio() const { return (double) m_n_accepted / m_n_generated; }
    [[nodiscard]] double average_time_per_call() const { return m_timer.cumulative_count() / m_n_calls; }

    void add_accepted_cut() { m_n_accepted++; }
    void add_effectiveness_statistics(double t_effectiveness);

    OsiCuts generate(OsiSolverInterface& t_osi_solver, int t_agressiveness);
};

#endif //IDOL_CUTFAMILY_H