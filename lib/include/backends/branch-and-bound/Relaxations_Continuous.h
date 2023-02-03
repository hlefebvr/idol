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
public:
    class Result : public Relaxation::Result {
        friend class Continuous;

        std::list<Var> m_branching_candidates;

        explicit Result(std::list<Var>&& t_branching_candidates) : m_branching_candidates(t_branching_candidates) {}
    public:
        [[nodiscard]] const std::list<Var>& branching_candidates() const { return m_branching_candidates; }
    };
private:
    Model m_model;
public:
    explicit Continuous(const Model& t_model) : m_model(t_model.clone()) {}

    Result build() {

        std::list<Var> branching_candidates;

        for (const auto& var : m_model.vars()) {

            const int type = m_model.get(Attr::Var::Type, var);

            if (type == Integer || type == Binary) {

                branching_candidates.emplace_back(var);
                m_model.set(Attr::Var::Type, var, ::Continuous);

            }

        }

        return Result(std::move(branching_candidates));

    }

    Model &model() override { return m_model; }

    [[nodiscard]] const Model &model() const override { return m_model; }
};

#endif //IDOL_RELAXATIONS_CONTINUOUS_H
