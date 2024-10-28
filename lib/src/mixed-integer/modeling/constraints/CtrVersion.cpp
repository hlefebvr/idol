//
// Created by henri on 28.10.24.
//
#include "idol/mixed-integer/modeling/constraints/CtrVersion.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"

idol::CtrVersion::CtrVersion(unsigned int t_index, TempCtr&& t_temp_ctr)
    : Version(t_index),
      m_type(t_temp_ctr.type()),
      m_rhs(t_temp_ctr.rhs()),
      m_lhs(std::make_unique<InternalLinExpr<Var>>(nullptr, std::move(t_temp_ctr.lhs()))) {

}

idol::CtrVersion::CtrVersion(unsigned int t_index, const TempCtr& t_temp_ctr) : CtrVersion(t_index, TempCtr(t_temp_ctr)) {

}
