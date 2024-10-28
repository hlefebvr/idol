//
// Created by henri on 28.10.24.
//
#include "idol/mixed-integer/modeling/variables/VarVersion.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"

idol::VarVersion::VarVersion(unsigned int t_index, double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr> &&t_column)
    : Version(t_index),
      m_lb(t_lb),
      m_ub(t_ub),
      m_type(t_type),
      m_obj(t_obj),
      m_column(std::make_unique<LinExpr<Ctr>>(std::move(t_column))) {

}

idol::VarVersion::VarVersion(unsigned int t_index, idol::TempVar &&t_temp_var)
    : VarVersion(
            t_index,
            t_temp_var.lb(),
            t_temp_var.ub(),
            t_temp_var.type(),
            t_temp_var.obj(),
            std::move(t_temp_var.column())) {

}

idol::VarVersion::VarVersion(unsigned int t_index, const idol::TempVar &t_temp_var)
    : VarVersion(
            t_index,
            t_temp_var.lb(),
            t_temp_var.ub(),
            t_temp_var.type(),
            t_temp_var.obj(),
            LinExpr<Ctr>(t_temp_var.column())) {

}

idol::VarVersion::VarVersion(const idol::VarVersion &t_other)
    : Version(t_other),
      m_lb(t_other.m_lb),
      m_ub(t_other.m_ub),
      m_type(t_other.m_type),
      m_obj(t_other.m_obj),
      m_column(t_other.m_column ? std::make_unique<LinExpr<Ctr>>(*t_other.m_column) : nullptr) {

}
