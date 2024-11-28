//
// Created by henri on 28.11.24.
//

#ifndef IDOL_DESCRIPTION_H
#define IDOL_DESCRIPTION_H

#include <optional>
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/expressions/AffExpr.h"

namespace idol::Robust {
    class Description;
}

class idol::Robust::Description {
    mutable std::optional<Annotation<unsigned int>> m_stages;
    Map<Ctr, LinExpr<Var, LinExpr<Var>>> m_uncertain_mat_coeff;
    Map<Ctr, LinExpr<Var>> m_uncertain_rhs;
    const Model& m_uncertainty_set;
public:
    explicit Description(const Model& t_uncertainty_set) : m_uncertainty_set(t_uncertainty_set) {}

    [[nodiscard]] const Annotation<unsigned int>& stage_annotation() const;

    void make_stage_var(const Var& t_var, unsigned int t_stage);

    void make_stage_ctr(const Ctr& t_ctr, unsigned int t_stage);

    unsigned int stage(const Var& t_var) const;

    unsigned int stage(const Ctr& t_var) const;

    const Model& uncertainty_set() const { return m_uncertainty_set; }

    auto uncertain_coefficients() const { return ConstIteratorForward(m_uncertain_mat_coeff); }

    const LinExpr<Var>& uncertain_mat_coeff(const Ctr& t_ctr, const Var& t_var) const;

    const LinExpr<Var, LinExpr<Var>>& uncertain_mat_coeffs(const Ctr& t_ctr) const;

    const LinExpr<Var>& uncertain_rhs(const Ctr& t_ctr) const;

    void set_uncertain_mat_coeff(const Ctr& t_ctr, const Var& t_var, const LinExpr<Var>& t_coeff) {
        m_uncertain_mat_coeff[t_ctr].set(t_var, t_coeff);
    }

    void set_uncertain_rhs(const Ctr& t_ctr, const LinExpr<Var>& t_rhs) {
        m_uncertain_rhs[t_ctr] = t_rhs;
    }

    class View {
        const Description& m_description;
        const Model& m_deterministic_model;
    public:
        View(const Model& t_deterministic_model, const Description& t_description);

        [[nodiscard]] const Description& description() const { return m_description; }

        [[nodiscard]] const Model& deterministic_model() const { return m_deterministic_model; }
    };
};

namespace idol {

    std::ostream &operator<<(std::ostream &t_os, const idol::Robust::Description::View &t_view);

}

#endif //IDOL_DESCRIPTION_H
