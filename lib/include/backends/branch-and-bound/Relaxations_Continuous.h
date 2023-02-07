//
// Created by henri on 03/02/23.
//

#ifndef IDOL_RELAXATIONS_CONTINUOUS_H
#define IDOL_RELAXATIONS_CONTINUOUS_H

#include "backends/Relaxation.h"

namespace Relaxations {
    class Continuous;
}

class Relaxations::Continuous : public Relaxation {
    const Model& m_original_model;
    std::optional<Model> m_relaxed_model;
    std::list<Var> m_branching_candidates;
public:
    explicit Continuous(const Model& t_model) : m_original_model(t_model) {}

    void build() override {

        m_branching_candidates.clear();

        m_relaxed_model.emplace(m_original_model.clone());

        for (const auto& var : m_relaxed_model->vars()) {

            const int type = m_relaxed_model->get(Attr::Var::Type, var);

            if (type == Integer || type == Binary) {

                m_branching_candidates.emplace_back(var);
                m_relaxed_model->set(Attr::Var::Type, var, ::Continuous);

            }

        }

    }

    [[nodiscard]] const std::list<Var>& branching_candidates() const { return m_branching_candidates; }

    Model &model() override { return *m_relaxed_model; }

    [[nodiscard]] const Model &model() const override { return *m_relaxed_model; }
};

#endif //IDOL_RELAXATIONS_CONTINUOUS_H
