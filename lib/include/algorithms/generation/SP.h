//
// Created by henri on 13/12/22.
//

#ifndef IDOL_SP_H
#define IDOL_SP_H

#include <memory>
#include <list>
#include "../Algorithm.h"
#include "../../modeling/models/Attributes_Model.h"
#include "../../modeling/variables/Attributes_Var.h"
#include "Pool.h"

template<class CRTP, class SubProblemT> class Generation;

template<class ParentT, class ObjectT>
class SP {
    using PresentGeneratorsList = std::list<std::pair<ObjectT, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

    bool m_is_local = true;

    // Parent generation algorithm
    ParentT& m_parent;

    // Solution strategies
    std::unique_ptr<Algorithm> m_exact_solution_strategy;

    // Generated objects
    Pool<ObjectT> m_pool;
    PresentGeneratorsList m_present_generators;
protected:
    ParentT& parent() { return m_parent; }
    const ParentT& parent() const { return m_parent; }

    Pool<ObjectT>& pool() { return m_pool; }
    virtual void remove_object_if(const std::function<bool(const ObjectT&, const Solution::Primal&)>& t_indicator_for_removal);

    virtual std::pair<ObjectT, Solution::Primal> enrich_master_problem_hook() = 0;
    virtual void update_local_bound_hook(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var& t_var, double t_value) = 0;
public:
    explicit SP(ParentT& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] bool has_exact_solution_strategy() const { return (bool) m_exact_solution_strategy; }

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }
    [[nodiscard]] const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&&... t_args);

    virtual void initialize() = 0;

    virtual void solve() {
        m_exact_solution_strategy->solve();
    }

    virtual void update() = 0;

    virtual bool can_enrich_master() {
        return m_exact_solution_strategy->get(Attr::Solution::ObjVal) < -ToleranceForAbsoluteGapPricing;
    }

    void enrich_master_problem() {
        auto [object, generator] = enrich_master_problem_hook();
        m_pool.add(object, std::move(generator));
        m_present_generators.emplace_back(object, m_pool.last_inserted());
    }

    virtual void update_local_bound(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var& t_var, double t_value) {

        if (t_attr == Attr::Var::Lb) {

            remove_object_if([t_value](const ObjectT& t_object, const Solution::Primal& t_generator) {
                return t_generator.get(t_object) < t_value;
            });

        } else {

            remove_object_if([t_value](const ObjectT& t_object, const Solution::Primal& t_generator) {
                return t_generator.get(t_object) > t_value;
            });

        }

        update_local_bound_hook(t_attr, t_var, t_value);
    }

    virtual void update_global_bound(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) {

        exact_solution_strategy().set(t_attr, t_var, t_value);

    }

    virtual void contribute_to_primal_solution(Solution::Primal& t_primal) const = 0;

    [[nodiscard]] PresentGenerators present_generators() const { return m_present_generators; }

    const Pool<ObjectT>& pool() const { return m_pool; }

    [[nodiscard]] bool is_local() const { return m_is_local; }

    void set_local(bool t_is_global) { m_is_local = t_is_global; }
};

template<class ParentT, class ObjectT>
void SP<ParentT, ObjectT>::remove_object_if(const std::function<bool(const ObjectT &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& rmp = parent().rmp_solution_strategy();

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            rmp.remove(column_variable);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }

}

template<class ParentT, class ObjectT>
template<class AlgorithmT, class... ArgsT>
AlgorithmT& SP<ParentT, ObjectT>::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* algorithm = new AlgorithmT(std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(algorithm);
    return *algorithm;
}

#endif //IDOL_SP_H
