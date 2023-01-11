//
// Created by henri on 11/01/23.
//
#include "modeling/models/Env.h"

std::unique_ptr<Env> Env::s_env;

Var Env::add_var(std::string &&t_name) {
    auto ref = m_variables.add_attributes(std::move(t_name), "Var");
    Var variable(std::move(ref));
    m_variables.add_object(variable);
    return variable;
}

Ctr Env::add_ctr(std::string &&t_name) {
    auto ref = m_constraints.add_attributes(std::move(t_name), "Ctr");
    Ctr constraint(std::move(ref));
    m_constraints.add_object(constraint);
    return constraint;
}

Env &Env::global() {
    if (!s_env) {
        s_env = std::make_unique<Env>();
    }
    return *s_env;
}
