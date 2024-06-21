//
// Created by henri on 21.06.24.
//

#ifndef IDOL_STAGEDESCRIPTION_H
#define IDOL_STAGEDESCRIPTION_H

#include "idol/modeling/variables/Var.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Versions.h"

namespace idol::Robust {
    class StageDescription;
}

class idol::Robust::StageDescription {
    Annotation<Var> m_stage_variables;
    Annotation<Ctr> m_stage_constraints;
public:
    StageDescription(Env& t_env, const std::string& t_name)
        : m_stage_variables(t_env, t_name + "_stage_variables", MasterId),
          m_stage_constraints(t_env, t_name + "_stage_constraints", MasterId) {}

    explicit StageDescription(Env& t_env) : StageDescription(t_env, "robust") {}

    StageDescription(const Annotation<Var>& t_stage_variables,
                     const Annotation<Ctr>& t_stage_constraints)
        : m_stage_variables(t_stage_variables),
          m_stage_constraints(t_stage_constraints) {}

    [[nodiscard]] const Annotation<Var>& stage_vars() const { return m_stage_variables; }

    [[nodiscard]] const Annotation<Ctr>& stage_ctrs() const { return m_stage_constraints; }

    void set_stage(const Var& t_var, unsigned int t_stage) {
        if (t_stage == 0) {
            throw Exception("Stage must be positive.");
        }
        t_var.set(m_stage_variables, t_stage - 2);
    }

    void set_stage(const Ctr& t_ctr, unsigned int t_stage) {
        if (t_stage == 0) {
            throw Exception("Stage must be positive.");
        }
        t_ctr.set(m_stage_constraints, t_stage - 2);
    }

    [[nodiscard]] unsigned int stage(const Var& t_var) const { return t_var.get(m_stage_variables) + 2; }

    [[nodiscard]] unsigned int stage(const Ctr& t_ctr) const { return t_ctr.get(m_stage_constraints) + 2; }

    [[nodiscard]] unsigned int stage_index(const Var& t_var) const { return t_var.get(m_stage_variables) + 1; }

    [[nodiscard]] unsigned int stage_index(const Ctr& t_ctr) const { return t_ctr.get(m_stage_constraints) + 1; }
};

#endif //IDOL_STAGEDESCRIPTION_H
