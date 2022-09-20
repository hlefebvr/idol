//
// Created by henri on 07/09/22.
//
#include "modeling/parameters/impl_Param.h"

impl::Param::Param(ObjectId &&t_id, const ::Var &t_variable)
    : impl::Object(std::move(t_id)), m_variable(t_variable) {

}
