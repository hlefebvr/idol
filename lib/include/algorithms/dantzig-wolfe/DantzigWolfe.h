//
// Created by henri on 14/12/22.
//

#ifndef IDOL_DANTZIGWOLFE_H
#define IDOL_DANTZIGWOLFE_H

#include "../../reformulations/Reformulations_DantzigWolfe.h"
#include "../Algorithm.h"
#include "DantzigWolfeSP.h"
#include "Parameters_DantzigWolfe.h"
#include "Attributes_DantzigWolfe.h"

class DantzigWolfe : public Algorithm {
public:
    enum Event { MasterProblemSolved, PricingProblemSolved, Begin, End };
    class Callback;
private:
    Reformulations::DantzigWolfe m_reformulation;
    std::vector<DantzigWolfeSP> m_subproblems;
    std::unique_ptr<Algorithm> m_master_solution_strategy;

    std::unique_ptr<Callback> m_callback;

    double m_best_lower_bound = -Inf;
    double m_best_upper_bound = +Inf;
    double m_last_lower_bound = -Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;

    std::list<Var> m_artificial_variables;
    bool m_current_is_farkas_pricing = false;

    std::optional<Solution::Dual> m_current_dual_solution;
    std::optional<Solution::Dual> m_adjusted_dual_solution;

    Param::DantzigWolfe::values<bool> m_bool_parameters;
    Param::DantzigWolfe::values<int> m_int_parameters;
    Param::DantzigWolfe::values<double> m_double_parameters;

    void call_callback(Event t_event);
protected:
    virtual void initialize();
    virtual void add_artificial_variables();
    virtual void solve_master_problem();
    void log_master_solution(bool t_force = false);
    void log_subproblem_solution(const DantzigWolfeSP& t_subproblem, bool t_force = false);
    virtual void analyze_master_problem_solution();
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution();
    virtual void enrich_master_problem();
    virtual void clean_up();
    virtual void analyze_feasibility_with_artificial_variables();
    virtual void remove_artificial_variables();
    [[nodiscard]] virtual bool stopping_condition() const;

    void execute() override;

    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;

    void set_original_formulation(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value);
    [[nodiscard]] double get_original_formulation(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const;
public:
    DantzigWolfe(Model& t_model, const UserAttr& t_complicating_constraint);

    Reformulations::DantzigWolfe& reformulation() { return m_reformulation; }
    [[nodiscard]] const Reformulations::DantzigWolfe& reformulation() const { return m_reformulation; }

    DantzigWolfeSP& subproblem(unsigned int t_index) { return m_subproblems[t_index-1]; }
    [[nodiscard]] const DantzigWolfeSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index-1]; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_master_solution_strategy(ArgsT&& ...t_args);

    template<class CallbackT, class ...ArgsT> CallbackT& set_callback(ArgsT&& ...t_args);

    Algorithm& master_solution_strategy() { return *m_master_solution_strategy; }
    [[nodiscard]] const Algorithm& master_solution_strategy() const { return *m_master_solution_strategy; }

    [[nodiscard]] Solution::Primal primal_solution() const override;
    [[nodiscard]] Solution::Dual dual_solution() const override;

    using Algorithm::set;
    using Algorithm::get;

    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;
    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const override;
    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    void set(const Parameter<int>& t_param, int t_value) override;
    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, void>& t_attr) const override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
    [[nodiscard]] int get(const Parameter<int>& t_param) const override;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &DantzigWolfe::set_master_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(m_reformulation.master_problem(), std::forward<ArgsT>(t_args)...);
    m_master_solution_strategy.reset(result);
    return *result;
}

template<class CallbackT, class... ArgsT>
CallbackT &DantzigWolfe::set_callback(ArgsT &&... t_args) {
    auto* result = new CallbackT(std::forward<ArgsT>(t_args)...);
    m_callback.reset(result);
    return *result;
}

#include "DantzigWolfe_Callback.h"

#endif //IDOL_DANTZIGWOLFE_H
