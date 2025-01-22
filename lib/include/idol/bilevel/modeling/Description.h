//
// Created by henri on 20.06.24.
//

#ifndef IDOL_BILEVEL_DESCRIPTION_H
#define IDOL_BILEVEL_DESCRIPTION_H

#include <utility>

#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Bilevel {
    class Description;
}

class idol::Bilevel::Description {
    Annotation<unsigned int> m_level;
    QuadExpr<Var> m_follower_objective;

    template<class T, unsigned int N>
    void set_annotation(const Vector<T, N> &t_vector, unsigned int t_value) {
        if constexpr (N == 1) {
            for (const auto& x : t_vector) {
                x.set(m_level, t_value);
            }
        } else  {
            for (const auto& x : t_vector) {
                add_vector<T, N - 1>(x);
            }
        }
    }

public:
    Description(Env& t_env, const std::string& t_name) : m_level(t_env, t_name + "_lower_level", MasterId)  {}

    explicit Description(Env& t_env) : Description(t_env, "bilevel") {}

    explicit Description(const Annotation<unsigned int>& t_lower_level) : m_level(t_lower_level) {}

    Description(const Annotation<unsigned int>& t_lower_level,
                AffExpr<Var> t_follower_objective)
        : m_level(t_lower_level),
          m_follower_objective(std::move(t_follower_objective)) {}

    [[nodiscard]] const Annotation<unsigned int>& lower_level() const { return m_level; }

    [[nodiscard]] const QuadExpr<Var>& follower_obj() const { return m_follower_objective; }

    void make_upper_level(const Var& t_var) { t_var.set(m_level, MasterId); }

    void make_upper_level(const Ctr& t_ctr) { t_ctr.set(m_level, MasterId); }

    void make_upper_level(const QCtr& t_ctr) { t_ctr.set(m_level, MasterId); }

    void make_lower_level(const Var& t_var) { t_var.set(m_level, 0); }

    template<unsigned int N> void make_lower_level(const Vector<Var, N>& t_vars) { set_annotation<Var, N>(t_vars, 0); }

    void make_lower_level(const Ctr& t_ctr) { t_ctr.set(m_level, 0); }

    void make_lower_level(const QCtr& t_ctr) { t_ctr.set(m_level, 0); }

    void set_lower_level_obj(QuadExpr<Var> t_objective) { m_follower_objective = std::move(t_objective); }

    [[nodiscard]] bool is_leader(const Var& t_var) const { return t_var.get(m_level) == MasterId; }

    [[nodiscard]] bool is_leader(const Ctr& t_ctr) const { return t_ctr.get(m_level) == MasterId; }

    [[nodiscard]] bool is_leader(const QCtr& t_ctr) const { return t_ctr.get(m_level) == MasterId; }

    [[nodiscard]] bool is_follower(const Var& t_var) const { return t_var.get(m_level) != MasterId; }

    [[nodiscard]] bool is_follower(const Ctr& t_ctr) const { return t_ctr.get(m_level) != MasterId; }

    [[nodiscard]] bool is_follower(const QCtr& t_ctr) const { return t_ctr.get(m_level) != MasterId; }
};

namespace idol::Bilevel {
    static bool is_coupling(const Model& t_model, const Description& t_description, const Ctr& t_ctr) {

        if (!t_description.is_leader(t_ctr)) {
            return false;
        }

        for (const auto& [var, constant] : t_model.get_ctr_row(t_ctr)) {
            if (t_description.is_follower(var)) {
                return true;
            }
        }

        return false;
    }
}

#endif //IDOL_BILEVEL_DESCRIPTION_H
