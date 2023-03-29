//
// Created by henri on 06/03/23.
//

#ifndef IDOL_ROWGENERATIONSP_H
#define IDOL_ROWGENERATIONSP_H

#include "modeling/models/Model.h"
#include "containers/GeneratorPool.h"

class RowGeneration;

namespace impl {
    class RowGenerationSP;
}

class impl::RowGenerationSP {
    using PresentGeneratorsList = std::list<std::pair<Ctr, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    RowGeneration* m_parent;
    const unsigned int m_index;
    std::unique_ptr<Model> m_model;
    ::Row m_generation_pattern;

    GeneratorPool<Ctr> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    RowGenerationSP(RowGeneration& t_parent, unsigned int t_index);

    void hook_before_solve();
    void update(const Solution::Primal& t_master_solution);
    void solve();
    void enrich_master_problem();
    [[nodiscard]] TempCtr create_cut_from_generator(const Solution::Primal& t_generator) const;
    void remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal);
    void apply_lb(const Var& t_var, double t_lb);
    void apply_ub(const Var& t_var, double t_ub);
public:
    virtual ~RowGenerationSP() = default;

    RowGenerationSP(const RowGenerationSP&) = delete;
    RowGenerationSP(RowGenerationSP&&) noexcept = default;

    RowGenerationSP& operator=(const RowGenerationSP&) = delete;
    RowGenerationSP& operator=(RowGenerationSP&&) = delete;

    [[nodiscard]] const Model& model() const { return *m_model; }
};

class RowGenerationSP : public impl::RowGenerationSP {
    friend class RowGeneration;
public:
    RowGenerationSP(RowGeneration& t_parent, unsigned int t_index);
};

#endif //IDOL_ROWGENERATIONSP_H
