//
// Created by henri on 20.06.24.
//

#ifndef IDOL_DESCRIPTION_H
#define IDOL_DESCRIPTION_H

#include <utility>

#include "idol/modeling/constraints/Ctr.h"
#include "idol/modeling/variables/Var.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/models/Model.h"

namespace idol::Bilevel {
    class Description;
}

class idol::Bilevel::Description {
    Annotation<Var> m_follower_variables;
    Annotation<Ctr> m_follower_constraints;
    Ctr m_follower_objective;
public:
    Description(Env& t_env, const std::string& t_name)
        : m_follower_variables(t_env, t_name + "_follower_variables", MasterId),
          m_follower_constraints(t_env, t_name + "_follower_constraints", MasterId),
          m_follower_objective(t_env, Equal, 0, t_name + "_follower_objective") {}

    explicit Description(Env& t_env) : Description(t_env, "bilevel") {}

    Description(const Annotation<Var>& t_follower_variables,
                const Annotation<Ctr>& t_follower_constraints,
                Ctr  t_follower_objective)
        : m_follower_variables(t_follower_variables),
          m_follower_constraints(t_follower_constraints),
          m_follower_objective(std::move(t_follower_objective)) {}

    [[nodiscard]] const Annotation<Var>& follower_vars() const { return m_follower_variables; }

    [[nodiscard]] const Annotation<Ctr>& follower_ctrs() const { return m_follower_constraints; }

    [[nodiscard]] const Ctr& follower_obj() const { return m_follower_objective; }

    void make_leader_var(const Var& t_var) { t_var.set(m_follower_variables, MasterId); }

    void make_leader_ctr(const Ctr& t_ctr) { t_ctr.set(m_follower_constraints, MasterId); }

    void make_follower_var(const Var& t_var) { t_var.set(m_follower_variables, 0); }

    void make_follower_ctr(const Ctr& t_ctr) { t_ctr.set(m_follower_constraints, 0); }

    void make_follower_objective(const Ctr& t_objective) { m_follower_objective = t_objective; }
};

#endif //IDOL_DESCRIPTION_H
