//
// Created by henri on 30/01/23.
//
#include "modeling/variables/Var.h"
#include "modeling/models/Env.h"

Var::Var(Env &t_env, double t_lb, double t_ub, int t_type, Column &&t_column, std::string t_name)
        : Object<VarVersion, Var>(t_env.create_var(std::move(t_name), TempVar(t_lb, t_ub, t_type, std::move(t_column)))) {

}

Var::Var(Env &t_env, double t_lb, double t_ub, int t_type, std::string t_name)
    : Var(t_env, t_lb, t_ub, t_type, Column(0.), std::move(t_name)) {

}

Var::Var(Env &t_env, double t_lb, double t_ub, int t_type, const Column &t_column, std::string t_name)
    : Var(t_env, t_lb, t_ub, t_type, Column(t_column), std::move(t_name)) {

}
