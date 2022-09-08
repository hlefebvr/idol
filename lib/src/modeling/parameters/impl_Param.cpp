//
// Created by henri on 07/09/22.
//
#include "modeling/parameters/impl_Param.h"

impl::Param::Param(ObjectId &&t_id, double t_lb, double t_ub, VarType t_type)
        : impl::Object(std::move(t_id)), m_lb(t_lb), m_ub(t_ub), m_type(t_type) {

}
