//
// Created by henri on 30/01/23.
//
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

idol::Var::Var(Env &t_env, double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr> &&t_column, std::string t_name)
        : Object<VarVersion, Var>(t_env.create_var(std::move(t_name), TempVar(t_lb, t_ub, t_type, t_obj, std::move(t_column)))) {

}

idol::Var::Var(Env &t_env, double t_lb, double t_ub, VarType t_type, double t_obj, std::string t_name)
    : Var(t_env, t_lb, t_ub, t_type, t_obj, LinExpr<Ctr>(), std::move(t_name)) {

}

idol::Var::Var(Env &t_env, double t_lb, double t_ub, VarType t_type, double t_obj, const LinExpr<Ctr> &t_column, std::string t_name)
    : Var(t_env, t_lb, t_ub, t_type, t_obj, LinExpr<Ctr>(t_column), std::move(t_name)) {

}

unsigned int idol::Var::index(const idol::Model &t_model) const {
    return t_model.get_var_index(*this);
}
