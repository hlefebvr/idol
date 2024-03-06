//
// Created by henri on 06.03.24.
//

#ifndef IDOL_CCGSTABILIZER_H
#define IDOL_CCGSTABILIZER_H

#include "idol/modeling/models/Model.h"

namespace idol::Optimizers::Robust {
    class ColumnAndConstraintGeneration;
}

namespace idol::Robust {
    class CCGStabilizer;
}

class idol::Robust::CCGStabilizer {
public:
    virtual ~CCGStabilizer() = default;

    class Strategy {
    private:
        idol::Optimizers::Robust::ColumnAndConstraintGeneration* m_parent = nullptr;
        void set_parent(idol::Optimizers::Robust::ColumnAndConstraintGeneration* t_parent) { m_parent = t_parent; }
    protected:
        idol::Optimizers::Robust::ColumnAndConstraintGeneration& parent() { return *m_parent; }
        [[nodiscard]] const idol::Optimizers::Robust::ColumnAndConstraintGeneration& parent() const { return *m_parent; }

        void set_status(SolutionStatus t_status);
        void set_reason(SolutionReason t_reason);
        void set_best_obj(double t_value);
        void set_best_bound(double t_value);
        void terminate();

        const idol::Model& master_problem() const;
        idol::Model& master_problem();

        const std::list<Var>& upper_level_vars() const;
        const Solution::Primal& current_master_solution() const;
        const Solution::Primal& current_separation_solution() const;
        void check_stopping_condition() const;
    public:
        virtual ~Strategy() = default;

        virtual void initialize() = 0;
        virtual void analyze_current_master_problem_solution() = 0;
        virtual void analyze_last_separation_solution() = 0;

        friend class idol::Optimizers::Robust::ColumnAndConstraintGeneration;
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual CCGStabilizer* clone() const = 0;
};

#endif //IDOL_CCGSTABILIZER_H
