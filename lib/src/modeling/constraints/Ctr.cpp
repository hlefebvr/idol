//
// Created by henri on 30/01/23.
//
#include "modeling/constraints/Ctr.h"
#include "modeling/models/Env.h"

Ctr::Ctr(Env &t_env, TempCtr &&t_temp_ctr, std::string t_name) : Object<CtrVersion>(t_env.create_ctr(std::move(t_name), std::move(t_temp_ctr))) {

}

Ctr::Ctr(Env &t_env, const TempCtr &t_temp_ctr, std::string t_name) : Ctr(t_env, TempCtr(t_temp_ctr), std::move(t_name)) {

}

Ctr::Ctr(Env &t_env, int t_type, std::string t_name) : Ctr(t_env, TempCtr(Row(0., 0.), t_type), std::move(t_name)) {

}

