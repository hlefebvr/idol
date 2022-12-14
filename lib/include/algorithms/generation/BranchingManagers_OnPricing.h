//
// Created by henri on 14/12/22.
//

#ifndef IDOL_BRANCHINGMANAGERS_ONPRICING_H
#define IDOL_BRANCHINGMANAGERS_ONPRICING_H

#include "BranchingManager.h"

class DantzigWolfeSP;

namespace BranchingManagers {
    class OnPricing;
}

class BranchingManagers::OnPricing : public BranchingManager {
    DantzigWolfeSP& m_parent;
public:
    explicit OnPricing(DantzigWolfeSP& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] double get_lb(const Var &t_var) const override;

    [[nodiscard]] double get_ub(const Var &t_var) const override;

    void set_lb(const Var &t_var, double t_lb) override;

    void set_ub(const Var &t_var, double t_ub) override;
};

#endif //IDOL_BRANCHINGMANAGERS_ONPRICING_H
