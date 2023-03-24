//
// Created by henri on 24/03/23.
//

#ifndef IDOL_COLUMNGENERATIONOPTIMIZERV2_H
#define IDOL_COLUMNGENERATIONOPTIMIZERV2_H

#include <vector>
#include "optimizers/OptimizerFactory.h"
#include "modeling/models/Model.h"

class ColumnGenerationOptimizerV2 : public OptimizerFactory {
public:
    class Subproblem;

    ColumnGenerationOptimizerV2(const OptimizerFactory& t_master_optimizer, unsigned int t_n_subproblems);

    Backend *operator()(const AbstractModel &t_model) const override;

    [[nodiscard]] ColumnGenerationOptimizerV2 *clone() const override;

    void reserve_subproblems(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    auto subproblems() { return IteratorForward(m_subproblems); }

    [[nodiscard]] auto subproblems() const { return ConstIteratorForward(m_subproblems); }

    Subproblem& subproblem(unsigned int t_index) { return m_subproblems[t_index]; }

    [[nodiscard]] const Subproblem& subproblem(unsigned int t_index) const { return m_subproblems[t_index]; }

    Subproblem& add_subproblem(const Model& t_model, Column t_column);

    ColumnGenerationOptimizerV2& with_subproblem(const Model& t_model, Column t_column);
private:
    std::vector<Subproblem> m_subproblems;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    ColumnGenerationOptimizerV2(const ColumnGenerationOptimizerV2& t_src);
};

class ColumnGenerationOptimizerV2::Subproblem {
    friend class ColumnGenerationOptimizerV2;

    unsigned int m_index = -1;
    const Model* m_model;
    Column m_generation_pattern;
    std::optional<Ctr> m_aggregator;

    Subproblem(const Subproblem& t_src) = default;
public:
    Subproblem(const Model& t_model, Column&& t_generation_pattern);

    Subproblem(Subproblem&&) noexcept = default;

    Subproblem& operator=(const Subproblem&) = delete;
    Subproblem& operator=(Subproblem&&) noexcept = default;

    [[nodiscard]] unsigned int index() const { return m_index; }

    [[nodiscard]] const Model& model() const { return *m_model; }

    [[nodiscard]] const Column& generation_pattern() const { return m_generation_pattern; }

    [[nodiscard]] bool has_aggregator() const { return m_aggregator.has_value(); }

    [[nodiscard]] Ctr aggregator() const { return m_aggregator.value(); }
};

#endif //IDOL_COLUMNGENERATIONOPTIMIZERV2_H
