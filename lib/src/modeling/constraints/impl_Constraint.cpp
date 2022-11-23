//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/constraints/impl_Constraint.h"
#include "../../../include/modeling/constraints/TempCtr.h"

impl::Ctr::Ctr(ObjectId &&t_id, CtrType t_type, Constant&& t_rhs)
    : impl::Object(std::move(t_id)),
      m_type(t_type),
      m_row(LinExpr(), std::move(t_rhs)) {

}

impl::Ctr::Ctr(ObjectId &&t_id, TempCtr &&t_temporary_constraint)
        : impl::Object(std::move(t_id)),
          m_row(std::move(t_temporary_constraint.m_row)),
          m_type(t_temporary_constraint.m_type) {

}

const Row &impl::Ctr::row() const {
    if (status() == Dead) {
        throw Exception("Trying to access row of removed constraint.");
    }
    return m_row;
}

Row &impl::Ctr::row() {
    if (status() == Dead) {
        throw Exception("Trying to access row of removed constraint.");
    }
    return m_row;
}

CtrType impl::Ctr::type() const {
    if (status() == Dead) {
        throw Exception("Trying to access type of removed constraint.");
    }
    return m_type;
}

