//
// Created by henri on 07/02/23.
//

#ifndef IDOL_COLUMNGENERATIONSP_H
#define IDOL_COLUMNGENERATIONSP_H

#include "modeling/models/AbstractModel.h"

namespace impl {
    class ColumnGenerationSP;
}

class impl::ColumnGenerationSP {
    std::unique_ptr<AbstractModel> m_model;
protected:
    explicit ColumnGenerationSP(AbstractModel* t_ptr) : m_model(t_ptr) {}

    [[nodiscard]] const AbstractModel& model() const { return *m_model; }

    void update(bool t_farkas_pricing, const Solution::Dual& t_duals) {
        throw Exception("TODO SP::update");
    }

    void solve() {
        throw Exception("TODO SP::solve");
    }

    [[nodiscard]] double compute_reduced_cost(const Solution::Dual& t_duals) const {
        throw Exception("TODO SP::comptue_reduced_cost");
    }

    void enrich_master_problem() {
        throw Exception("TODO SP::enrich_master_problem");
    }

    void clean_up() {
        throw Exception("TODO SP::clean_up");
    }
};

class ColumnGenerationSP : public impl::ColumnGenerationSP {
    friend class ColumnGeneration;
public:
    explicit ColumnGenerationSP(AbstractModel* t_ptr) : impl::ColumnGenerationSP(t_ptr) {}
};

#endif //IDOL_COLUMNGENERATIONSP_H
