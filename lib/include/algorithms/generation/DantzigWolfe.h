//
// Created by henri on 14/12/22.
//

#ifndef IDOL_DANTZIGWOLFE_H
#define IDOL_DANTZIGWOLFE_H

#include "../../reformulations/Reformulations_DantzigWolfe.h"
#include "../Algorithm.h"
#include "DantzigWolfeSP.h"
#include "Parameters_DantzigWolfe.h"

class DantzigWolfe : public Algorithm {
    Reformulations::DantzigWolfe m_reformulation;
    std::vector<DantzigWolfeSP> m_subproblems;
    std::unique_ptr<Algorithm> m_master_solution_strategy;
protected:
    virtual void initialize();
    virtual void solve_master_problem();
    virtual void analyze_master_problem_solution();
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution();
    virtual void enrich_master_problem();

    void execute() override;

    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;

    void set_original_formulation(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value);
    double get_original_formulation(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const;
public:
    DantzigWolfe(Model& t_model, const UserAttr& t_complicating_constraint);

    Reformulations::DantzigWolfe& reformulation() { return m_reformulation; }
    const Reformulations::DantzigWolfe& reformulation() const { return m_reformulation; }

    DantzigWolfeSP& subproblem(unsigned int t_index) { return m_subproblems[t_index-1]; }
    const DantzigWolfeSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index-1]; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_master_solution_strategy(ArgsT&& ...t_args);

    Algorithm& master_solution_strategy() { return *m_master_solution_strategy; }
    const Algorithm& master_solution_strategy() const { return *m_master_solution_strategy; }

    Solution::Primal primal_solution() const override;
    Solution::Dual dual_solution() const override;

    using Algorithm::set;
    using Algorithm::get;

    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;
    double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const override;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &DantzigWolfe::set_master_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(m_reformulation.master_problem(), std::forward<ArgsT>(t_args)...);
    m_master_solution_strategy.reset(result);
    return *result;
}

#endif //IDOL_DANTZIGWOLFE_H
