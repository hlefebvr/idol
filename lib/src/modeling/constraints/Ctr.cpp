//
// Created by henri on 30/01/23.
//
#include "modeling/constraints/Ctr.h"
#include "modeling/objects/Env.h"

Ctr::Ctr(Env &t_env, TempCtr &&t_temp_ctr, std::string t_name) : Object<CtrVersion, Ctr>(t_env.create_ctr(std::move(t_name), std::move(t_temp_ctr))) {

}

Ctr::Ctr(Env &t_env, const TempCtr &t_temp_ctr, std::string t_name) : Ctr(t_env, TempCtr(t_temp_ctr), std::move(t_name)) {

}

Ctr::Ctr(Env &t_env, int t_type, Constant&& t_constant, std::string t_name) : Ctr(t_env, TempCtr(Row(0., std::move(t_constant)), t_type), std::move(t_name)) {

}

Ctr::Ctr(Env &t_env, int t_type, const Constant &t_constant, std::string t_name) : Ctr(t_env, t_type, Constant(t_constant), std::move(t_name)) {

}
