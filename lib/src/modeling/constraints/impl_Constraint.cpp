//
// Created by henri on 07/09/22.
//
#include "modeling/constraints/impl_Constraint.h"
#include "modeling/constraints/TempCtr.h"

impl::Ctr::Ctr(ObjectId &&t_id, CtrType t_type, Coefficient&& t_rhs)
    : impl::Object(std::move(t_id)),
      m_type(t_type),
      m_row(std::move(t_rhs)) {

}

const Row &impl::Ctr::row() const {
    return m_row;
}

Row &impl::Ctr::row() {
    return m_row;
}

impl::Ctr::Ctr(ObjectId &&t_id, TempCtr &&t_temporary_constraint)
    : impl::Object(std::move(t_id)),
      m_row(std::move(t_temporary_constraint.m_row)),
      m_type(t_temporary_constraint.m_type) {

}

CtrType impl::Ctr::type() const {
    return m_type;
}

