//
// Created by henri on 30/01/23.
//
#include "idol/modeling//constraints/Ctr.h"
#include "idol/modeling//objects/Env.h"

idol::Ctr::Ctr(Env &t_env, TempCtr &&t_temp_ctr, std::string t_name) : Object<CtrVersion, Ctr>(t_env.create_ctr(std::move(t_name), std::move(t_temp_ctr))) {

}

idol::Ctr::Ctr(Env &t_env, const TempCtr &t_temp_ctr, std::string t_name) : Ctr(t_env, TempCtr(t_temp_ctr), std::move(t_name)) {

}

idol::Ctr::Ctr(Env &t_env, CtrType t_type, Constant&& t_constant, std::string t_name) : Ctr(t_env, TempCtr(Row(0., std::move(t_constant)), t_type), std::move(t_name)) {

}

idol::Ctr::Ctr(Env &t_env, CtrType t_type, const Constant &t_constant, std::string t_name) : Ctr(t_env, t_type, Constant(t_constant), std::move(t_name)) {

}
