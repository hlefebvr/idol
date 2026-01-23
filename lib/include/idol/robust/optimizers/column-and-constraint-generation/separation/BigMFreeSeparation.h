//
// Created by Henri on 21/01/2026.
//

#ifndef IDOL_CCG_BIGMFREESEPARATION_H
#define IDOL_CCG_BIGMFREESEPARATION_H
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/Separation.h"

namespace idol::Robust::CCG {
    class BigMFreeSeparation;
}

namespace idol {
    static double overestimate(const AffExpr<Var>& t_expr,
        const std::function<double(const Var&)>& t_get_var_lb,
        const std::function<double(const Var&)>& t_get_var_ub);
    static double underestimate(const AffExpr<Var>& t_expr,
        const std::function<double(const Var&)>& t_get_var_lb,
        const std::function<double(const Var&)>& t_get_var_ub);
}

class idol::Robust::CCG::BigMFreeSeparation : public idol::Robust::CCG::Separation {
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
    double m_M = 1e2;
    double m_kappa = 0;
    std::vector<Var> m_slack_for_constraints;
    std::optional<Var> m_slack_for_objective;

    BigMFreeSeparation(const BigMFreeSeparation& t_src);
    void compute_kappa(const Model& t_separation_model, const Bilevel::Description& t_separation_bilevel_description);
public:
    BigMFreeSeparation() = default;

    void operator()() override;
    [[nodiscard]] Separation* clone() const override { return new BigMFreeSeparation(*this); }

    BigMFreeSeparation& with_single_level_optimizer(const OptimizerFactory& t_optimizer);

    std::pair<idol::Model, idol::Bilevel::Description> build_separation_problem() override;

    class BoundProvider : public idol::Reformulators::KKT::BoundProvider {
        const BigMFreeSeparation& m_parent;
    public:
        BoundProvider(const BigMFreeSeparation& t_parent);

        double get_var_lb(const Var& t_var) override;
        double get_var_ub(const Var& t_var) override;
        double get_ctr_dual_lb(const Ctr &t_ctr) override;
        double get_ctr_dual_ub(const Ctr &t_ctr) override;
        double get_ctr_slack_lb(const Ctr &t_ctr) override;
        double get_ctr_slack_ub(const Ctr &t_ctr) override;
        double get_var_lb_dual_ub(const Var &t_var) override;
        double get_var_ub_dual_lb(const Var &t_var) override;
        [[nodiscard]] BoundProvider *clone() const override { return new BoundProvider(*this); }
    };
};

#endif //IDOL_CCG_BIGMFREESEPARATION_H