//
// Created by henri on 06.03.24.
//

#ifndef IDOL_TRUSTREGION_H
#define IDOL_TRUSTREGION_H

#include "CCGStabilizer.h"

namespace idol::Robust::CCGStabilizers {
    class TrustRegion;
}

class idol::Robust::CCGStabilizers::TrustRegion : public idol::Robust::CCGStabilizer {
    std::optional<std::vector<double>> m_trust_factors;
public:
    TrustRegion() = default;

    TrustRegion(const TrustRegion& t_other) = default;

    class Strategy : public CCGStabilizer::Strategy {
        std::vector<double> m_trust_factor;
        unsigned int m_current_trust_factor_index = 0;
        std::optional<Solution::Primal> m_stability_center;
        std::optional<Ctr> m_local_branching_constraint;
        std::list<std::tuple<Ctr, Solution::Primal, unsigned int>> m_reversed_local_branching_constraints;
        unsigned int m_n_binary_first_stage_decisions = 0;
    public:
        Strategy(const std::vector<double>& t_trust_factors);

        void initialize() override;

        void analyze_current_master_problem_solution() override;

        void analyze_last_separation_solution() override;

        Expr<Var, Var> build_distance_to_point(const Solution::Primal& t_point);

        void update_local_branching_constraint();

        void add_reversed_local_branching_constraint();

        void remove_all_stabilization_constraints(Model& t_model);

        unsigned int current_radius() const;

        void update_radius();
    };

    Strategy *operator()() const override;

    TrustRegion *clone() const override;

    TrustRegion& with_trust_factors(std::vector<double> t_trust_factors);
};

#endif //IDOL_TRUSTREGION_H
