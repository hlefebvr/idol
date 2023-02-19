//
// Created by henri on 07/02/23.
//

#ifndef IDOL_COLUMNGENERATIONSP_H
#define IDOL_COLUMNGENERATIONSP_H

#include "modeling/models/AbstractModel.h"
#include "containers/GeneratorPool.h"
#include "BranchingManager.h"

class ColumnGeneration;

namespace impl {
    class ColumnGenerationSP;
}

class impl::ColumnGenerationSP {
    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    ColumnGeneration& m_parent;
    const unsigned int m_index;
    std::unique_ptr<AbstractModel> m_model;
    ::Column m_generation_pattern;

    // Branching on master
    Map<Var, Ctr> m_lower_bound_constraints;
    Map<Var, Ctr> m_upper_bound_constraints;
    void apply_bound_on_master(const Var& t_var, const Req<double, Var>& t_bound, double t_value);

    GeneratorPool<Var> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    explicit ColumnGenerationSP(ColumnGeneration& t_parent, unsigned int t_index);

    void update_objective(bool t_farkas_pricing, const Solution::Dual& t_duals);

    void hook_before_solve();

    void solve();

    [[nodiscard]] double compute_reduced_cost(const Solution::Dual& t_duals) const;

    [[nodiscard]] double compute_original_space_primal(const Var& t_var) const;

    [[nodiscard]] TempVar create_column_from_generator(const Solution::Primal& t_primals) const;

    void apply_lb(const Var& t_var, double t_value);

    void apply_ub(const Var& t_var, double t_value);

    void enrich_master_problem();

    void remove_column_if(const std::function<bool(const Var&, const Solution::Primal&)>& t_indicator_for_removal);

    void restore_column_from_pool();

    void clean_up();
public:
    [[nodiscard]] const AbstractModel& model() const { return *m_model; }

    [[nodiscard]] auto present_generators() const { return PresentGenerators(m_present_generators); }

    [[nodiscard]] LinExpr<Var> expand(const Var& t_var) const;
};

class ColumnGenerationSP : public impl::ColumnGenerationSP {
    friend class ColumnGeneration;
public:
    explicit ColumnGenerationSP(ColumnGeneration &t_parent, unsigned int t_index) : impl::ColumnGenerationSP(t_parent, t_index) {}
};

#endif //IDOL_COLUMNGENERATIONSP_H
