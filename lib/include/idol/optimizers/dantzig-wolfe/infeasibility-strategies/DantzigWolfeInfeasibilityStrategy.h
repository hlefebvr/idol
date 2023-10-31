//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEINFEASIBILITYSTRATEGY_H
#define IDOL_DANTZIGWOLFEINFEASIBILITYSTRATEGY_H

#include "idol/modeling/solutions/types.h"
#include "idol/modeling/solutions/Solution.h"

namespace idol::Optimizers {
    class DantzigWolfeDecomposition;
}

namespace idol::DantzigWolfe {
    class InfeasibilityStrategyFactory;
}

class idol::DantzigWolfe::InfeasibilityStrategyFactory {
public:
    virtual ~InfeasibilityStrategyFactory() = default;

    class Strategy {
        SolutionStatus m_status = Loaded;
        SolutionReason m_reason = NotSpecified;
        std::optional<double> m_best_obj;
        std::optional<double> m_best_bound;
        std::optional<Solution::Primal> m_primal_solution;
    public:
        virtual ~Strategy() = default;

        virtual void execute(Optimizers::DantzigWolfeDecomposition& t_parent) = 0;

        SolutionStatus status() const { return m_status; }

        SolutionReason reason() const { return m_reason; }

        double best_obj() const { return m_best_obj.value(); }

        double best_bound() const { return m_best_bound.value(); }

        const Solution::Primal& primal_solution() const;
    protected:
        void set_status(SolutionStatus t_status) { m_status = t_status; }

        void set_reason(SolutionReason t_reason) { m_reason = t_reason; }

        void set_primal_solution(Solution::Primal t_solution) { m_primal_solution = std::move(t_solution); }

        void set_best_obj(double t_best_obj) { m_best_obj = t_best_obj; }

        void set_best_bound(double t_best_bound) { m_best_bound = t_best_bound; }
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual InfeasibilityStrategyFactory* clone() const = 0;
};

#endif //IDOL_DANTZIGWOLFEINFEASIBILITYSTRATEGY_H
