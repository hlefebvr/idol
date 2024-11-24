//
// Created by henri on 20.06.24.
//

#ifndef IDOL_DESCRIPTION_H
#define IDOL_DESCRIPTION_H

#include <utility>

#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Bilevel {
    class Description;
}

class idol::Bilevel::Description {
    Annotation<unsigned int> m_lower_level;
    AffExpr<Var> m_follower_objective;
public:
    Description(Env& t_env, const std::string& t_name) : m_lower_level(t_env, t_name + "_lower_level", MasterId)  {}

    explicit Description(Env& t_env) : Description(t_env, "bilevel") {}

    explicit Description(const Annotation<unsigned int>& t_lower_level) : m_lower_level(t_lower_level) {}

    Description(const Annotation<unsigned int>& t_lower_level,
                AffExpr<Var> t_follower_objective)
        : m_lower_level(t_lower_level),
          m_follower_objective(std::move(t_follower_objective)) {}

    [[nodiscard]] const Annotation<unsigned int>& lower_level() const { return m_lower_level; }

    [[nodiscard]] const AffExpr<Var>& follower_obj() const { return m_follower_objective; }

    void make_leader(const Var& t_var) { t_var.set(m_lower_level, MasterId); }

    void make_leader(const Ctr& t_ctr) { t_ctr.set(m_lower_level, MasterId); }

    void make_leader(const QCtr& t_ctr) { t_ctr.set(m_lower_level, MasterId); }

    void make_lower_level(const Var& t_var) { t_var.set(m_lower_level, 0); }

    void make_follower(const Ctr& t_ctr) { t_ctr.set(m_lower_level, 0); }

    void make_lower_level(const QCtr& t_ctr) { t_ctr.set(m_lower_level, 0); }

    void set_lower_objective(AffExpr<Var> t_objective) { m_follower_objective = std::move(t_objective); }

    [[nodiscard]] bool is_leader(const Var& t_var) const { return t_var.get(m_lower_level) == MasterId; }

    [[nodiscard]] bool is_leader(const Ctr& t_ctr) const { return t_ctr.get(m_lower_level) == MasterId; }

    [[nodiscard]] bool is_leader(const QCtr& t_ctr) const { return t_ctr.get(m_lower_level) == MasterId; }

    [[nodiscard]] bool is_follower(const Var& t_var) const { return t_var.get(m_lower_level) != MasterId; }

    [[nodiscard]] bool is_follower(const Ctr& t_ctr) const { return t_ctr.get(m_lower_level) != MasterId; }

    [[nodiscard]] bool is_follower(const QCtr& t_ctr) const { return t_ctr.get(m_lower_level) != MasterId; }
};

#endif //IDOL_DESCRIPTION_H