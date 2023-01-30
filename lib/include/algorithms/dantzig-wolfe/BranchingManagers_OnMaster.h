//
// Created by henri on 14/12/22.
//

#ifndef IDOL_BRANCHINGMANAGERS_ONMASTER_H
#define IDOL_BRANCHINGMANAGERS_ONMASTER_H

#include "BranchingManager.h"
#include "../../modeling/attributes/Attributes.h"
#include "../../modeling/variables/Var.h"
#include "../../modeling/constraints/Ctr.h"
#include "../../containers/Map.h"

namespace BranchingManagers {
    class OnMaster;
}

class DantzigWolfeSP;

class BranchingManagers::OnMaster : public BranchingManager {
    DantzigWolfeSP &m_parent;

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    [[nodiscard]] double get_bound(const Map<Var, Ctr>& t_container, const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const;
    void set_bound(Map<Var, Ctr>& t_container, const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value);
public:
    explicit OnMaster(DantzigWolfeSP &t_parent) : m_parent(t_parent) {}

    [[nodiscard]] double get_lb(const Var &t_var) const override;

    [[nodiscard]] double get_ub(const Var &t_var) const override;

    void set_lb(const Var &t_var, double t_lb) override;

    void set_ub(const Var &t_var, double t_ub) override;
};

#endif //IDOL_BRANCHINGMANAGERS_ONMASTER_H
