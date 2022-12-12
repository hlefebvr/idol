//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITION_H
#define OPTIMIZE_DECOMPOSITION_H

#include "GenerationAlgorithm.h"
#include <functional>
#include <memory>

class Decomposition : public Algorithm {
    std::unique_ptr<Algorithm> m_rmp_strategy;
    std::list<std::unique_ptr<Algorithm>> m_generation_strategies;
protected:
    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;

    void execute() override;
public:
    Decomposition() = default;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void update_var_lb(const Var &t_var, double t_lb) override;

    void update_var_ub(const Var &t_var, double t_ub) override;

    void update_rhs_coeff(const Ctr &t_ctr, double t_rhs) override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;

    void remove(const Ctr &t_constraint) override;

    template<class T, class ...Args> T& set_rmp_solution_strategy(Args&& ...t_args) {
        auto* solution_strategy = new T(std::forward<Args>(t_args)...);
        m_rmp_strategy.reset(solution_strategy);
        m_rmp_strategy->set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);
        return *solution_strategy;
    }

    template<class T, class ...Args> T& add_generation_strategy(Args&& ...t_args) {
        auto* rmp_strategy = m_generation_strategies.empty() ? m_rmp_strategy.get() : m_generation_strategies.back().get();
        auto* generation_strategy = new T(*rmp_strategy, std::forward<Args>(t_args)...);
        m_generation_strategies.emplace_back(generation_strategy);
        return *generation_strategy;
    }

    void set(const Parameter<double> &t_param, double t_value) override;

    void set(const Parameter<bool> &t_param, bool t_value) override;

    void set(const Parameter<int> &t_param, int t_value) override;

    double get(const Parameter<double> &t_param) const override;

    bool get(const Parameter<bool> &t_param) const override;

    int get(const Parameter<int> &t_param) const override;

    class iterator {
        friend class Decomposition;
        std::list<std::unique_ptr<Algorithm>>::iterator m_it;
        explicit iterator(std::list<std::unique_ptr<Algorithm>>::iterator t_it) : m_it(t_it) {}
    public:
        bool operator!=(const iterator& t_rhs) const { return m_it != t_rhs.m_it; }
        iterator& operator++() { ++m_it; return *this; }
        Algorithm& operator*() { return **m_it; }
        const Algorithm& operator*() const { return **m_it; }
        Algorithm* operator->() { return m_it->get(); }
        const Algorithm* operator->() const { return m_it->get(); }
    };

    class const_iterator {
        friend class Decomposition;
        std::list<std::unique_ptr<Algorithm>>::const_iterator m_it;
        explicit const_iterator(std::list<std::unique_ptr<Algorithm>>::const_iterator t_it) : m_it(t_it) {}
    public:
        bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
        const_iterator& operator++() { ++m_it; return *this; }
        const Algorithm& operator*() const { return **m_it; }
        const Algorithm* operator->() const { return m_it->get(); }
    };

    iterator begin() { return iterator(m_generation_strategies.begin()); }
    iterator end() { return iterator(m_generation_strategies.end()); }

    const_iterator cbegin() const { return const_iterator(m_generation_strategies.begin()); }
    const_iterator cend() const { return const_iterator(m_generation_strategies.end()); }

    const_iterator begin() const { return const_iterator(m_generation_strategies.begin()); }
    const_iterator end() const { return const_iterator(m_generation_strategies.end()); }
};

#endif //OPTIMIZE_DECOMPOSITION_H
