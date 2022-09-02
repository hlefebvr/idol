//
// Created by henri on 01/09/22.
//

#include "../../include/modeling/impl_Variable.h"

impl::Variable::Variable(Env& t_env, unsigned int t_index, double t_lower_bound, double t_upper_bound, VariableType t_type, std::string&& t_name)
    : Object(t_env, t_index, t_name.empty() ? "Var(" + std::to_string(id()) + ")" : std::move(t_name)),
      m_lower_bound(t_lower_bound),
      m_upper_bound(t_upper_bound),
      m_type(t_type) {

}

