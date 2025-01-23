//
// Created by henri on 19.11.24.
//
#include "idol/mixed-integer/modeling/constraints/QCtrVersion.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"

idol::QCtrVersion::QCtrVersion(unsigned int t_index, TempQCtr &&t_temp_ctr)
    : Version(t_index),
      m_type(t_temp_ctr.type()),
      m_expr(std::move(t_temp_ctr.expr())) {

}

idol::QCtrVersion::QCtrVersion(unsigned int t_index, const idol::TempQCtr &t_temp_ctr)
    : QCtrVersion(t_index, TempQCtr(t_temp_ctr)) {

}
