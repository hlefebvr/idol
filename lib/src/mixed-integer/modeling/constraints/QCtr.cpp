//
// Created by henri on 19.11.24.
//
#include "idol/mixed-integer/modeling/constraints/QCtr.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

idol::QCtr::QCtr(Env &t_env, TempQCtr &&t_temp_ctr, std::string t_name)
    : Object<QCtrVersion, QCtr>(t_env.create_qctr(std::move(t_name), std::move(t_temp_ctr))) {

}

idol::QCtr::QCtr(idol::Env &t_env, const idol::TempQCtr &t_temp_ctr, std::string t_name)
    : QCtr(t_env, TempQCtr(t_temp_ctr), std::move(t_name)) {

}

idol::QCtr::QCtr(idol::Env &t_env, idol::CtrType t_type, std::string t_name)
    : QCtr(t_env, TempQCtr(LinExpr<Var>(), t_type), std::move(t_name)) {

}

unsigned int idol::QCtr::index(const idol::Model &t_model) const {
    return t_model.get_qctr_index(*this);
}

