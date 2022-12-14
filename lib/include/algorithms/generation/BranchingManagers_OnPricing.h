//
// Created by henri on 14/12/22.
//

#ifndef IDOL_BRANCHINGMANAGERS_ONPRICING_H
#define IDOL_BRANCHINGMANAGERS_ONPRICING_H

#include "BranchingManager.h"

namespace BranchingManagers {
    class OnPricing;
}

class BranchingManagers::OnPricing : public BranchingManager {
    DantzigWolfeSP& m_parent;
public:
    explicit OnPricing(DantzigWolfeSP& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] double get_lb(const Var &t_var) const override {
        return m_parent.exact_solution_strategy().get(Attr::Var::Lb, t_var);
    }

    [[nodiscard]] double get_ub(const Var &t_var) const override {
        return m_parent.exact_solution_strategy().get(Attr::Var::Ub, t_var);
    }

    void set_lb(const Var &t_var, double t_lb) override {
        m_parent.exact_solution_strategy().set(Attr::Var::Lb, t_var, t_lb);
    }

    void set_ub(const Var &t_var, double t_ub) override {
        m_parent.exact_solution_strategy().set(Attr::Var::Ub, t_var, t_ub);
    }
};

#endif //IDOL_BRANCHINGMANAGERS_ONPRICING_H
