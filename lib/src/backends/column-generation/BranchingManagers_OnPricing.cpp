//
// Created by henri on 14/12/22.
//
#include "backends/column-generation/BranchingManagers_OnPricing.h"
#include "backends/column-generation/ColumnGenerationSP.h"
#include "modeling/variables/Attributes_Var.h"

double BranchingManagers::OnPricing::get_lb(const Var &t_var) const {
    return m_parent.model().get(Attr::Var::Lb, t_var);
}

double BranchingManagers::OnPricing::get_ub(const Var &t_var) const {
    return m_parent.model().get(Attr::Var::Ub, t_var);
}

void BranchingManagers::OnPricing::set_lb(const Var &t_var, double t_lb) {
    m_parent.model().set(Attr::Var::Lb, t_var, t_lb);
}

void BranchingManagers::OnPricing::set_ub(const Var &t_var, double t_ub) {
    m_parent.model().set(Attr::Var::Ub, t_var, t_ub);
}

