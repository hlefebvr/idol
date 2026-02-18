//
// Created by Henri on 21/01/2026.
//

#ifndef IDOL_CCG_SEPARATION_H
#define IDOL_CCG_SEPARATION_H
#include "idol/general/utils/IteratorForward.h"
#include "idol/general/utils/Point.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol {
    namespace Bilevel {
        class Description;
    }

    namespace CCG {
        class Formulation;
    }

    namespace Robust::CCG {
        class Separation;
    }

    namespace Optimizers::Robust {
        class ColumnAndConstraintGeneration;
    }
}

class idol::Robust::CCG::Separation {
    std::vector<PrimalPoint> m_scenarios;
    std::optional<double> m_submitted_upper_bound;
    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    idol::Optimizers::Robust::ColumnAndConstraintGeneration* m_parent = nullptr;
protected:
    virtual void operator()() = 0;
    void submit_upper_bound(double t_value);
    void add_scenario(PrimalPoint t_scenario) { m_scenarios.emplace_back(std::move(t_scenario)); }
    void clear_scenarios() { m_scenarios.clear(); }
    void terminate();
    void set_status(SolutionStatus t_status, SolutionReason t_reason);

    [[nodiscard]] const PrimalPoint& get_master_solution() const;
    [[nodiscard]] Env& get_env() const;
    [[nodiscard]] const idol::Optimizers::Robust::ColumnAndConstraintGeneration& get_parent() const { return *m_parent; }
    [[nodiscard]] const idol::Robust::Description& get_robust_description() const;
    [[nodiscard]] const idol::Bilevel::Description& get_bilevel_description() const;
    [[nodiscard]] const idol::CCG::Formulation& get_formulation() const;
public:
    virtual ~Separation() = default;

    [[nodiscard]] auto scenarios() const { return ConstIteratorForward(m_scenarios); }

    [[nodiscard]] virtual Separation* clone() const = 0;

    virtual std::pair<idol::Model, idol::Bilevel::Description> build_separation_problem();

    friend class idol::Optimizers::Robust::ColumnAndConstraintGeneration;
};

#endif //IDOL_CCG_SEPARATION_H