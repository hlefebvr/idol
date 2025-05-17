//
// Created by henri on 08.05.25.
//

#ifndef IDOL_ADVERSARIALPROBLEMSOLVER_H
#define IDOL_ADVERSARIALPROBLEMSOLVER_H

#include "Formulation.h"

namespace idol::Optimizers::Robust::impl {
    class AdversarialProblemSolver;
}

class idol::Optimizers::Robust::impl::AdversarialProblemSolver {
    const ::idol::CCG::Formulation& m_formulation;

    bool m_has_failed = false;
    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    std::vector<Point<Var>> m_scenarios;
public:
    explicit AdversarialProblemSolver(const ::idol::CCG::Formulation& t_formulation) : m_formulation(t_formulation) {}

    [[nodiscard]] bool has_failed() const { return m_has_failed; }
    [[nodiscard]] SolutionStatus status() const { return m_status; }
    [[nodiscard]] SolutionReason reason() const { return m_reason; }
    [[nodiscard]] const std::vector<Point<Var>>& scenarios() const { return m_scenarios; }

    void set_status(SolutionStatus t_status) { m_status = t_status; }
    void set_reason(SolutionReason t_reason) { m_reason = t_reason; }

    void operator()(const Point<Var>& t_first_stage_solution);
};

#endif //IDOL_ADVERSARIALPROBLEMSOLVER_H
