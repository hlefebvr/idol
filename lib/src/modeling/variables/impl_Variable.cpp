//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/variables/impl_Variable.h"
#include "../../../include/modeling/variables/TempVar.h"

impl::Var::Var(ObjectId &&t_id, double t_lb, double t_ub, VarType t_type, Column&& t_column)
    : impl::Object(std::move(t_id)), m_lb(t_lb), m_ub(t_ub), m_type(t_type), m_column(std::move(t_column)) {

}

impl::Var::Var(ObjectId &&t_id, TempVar &&t_temporary_variable)
        : impl::Object(std::move(t_id)),
          m_lb(t_temporary_variable.m_lb),
          m_ub(t_temporary_variable.m_ub),
          m_type(t_temporary_variable.m_type),
          m_column(std::move(t_temporary_variable.m_column)) {

}

const Column &impl::Var::column() const {
    return m_column;
}

Column &impl::Var::column() {
    return m_column;
}
