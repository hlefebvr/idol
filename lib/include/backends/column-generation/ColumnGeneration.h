//
// Created by henri on 07/02/23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include "../Algorithm.h"
#include "modeling/models/BlockModel.h"
#include "Parameters_ColumnGeneration.h"
#include "ColumnGenerationSP.h"

class ColumnGeneration : public Algorithm {
    std::unique_ptr<AbstractModel> m_master;
    std::vector<ColumnGenerationSP> m_subproblems;

    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;

    std::list<Var> m_artificial_variables;
    bool m_current_is_farkas_pricing = false;

    std::optional<Solution::Dual> m_current_dual_solution;
    std::optional<Solution::Dual> m_adjusted_dual_solution;

    Param::ColumnGeneration::values<bool> m_bool_parameters;
    Param::ColumnGeneration::values<int> m_int_parameters;
    Param::ColumnGeneration::values<double> m_double_parameters;
protected:
    void initialize() override;

    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    virtual void add_artificial_variables();
    virtual void solve_master_problem();
    void log_master_solution(bool t_force = false) const;
    void log_subproblem_solution(const ColumnGenerationSP& t_subproblem, bool t_force = false) const;
    virtual void analyze_master_problem_solution();
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution();
    virtual void enrich_master_problem();
    virtual void clean_up();
    virtual void analyze_feasibility_with_artificial_variables();
    virtual void remove_artificial_variables();
    [[nodiscard]] virtual bool stopping_condition() const;

    void hook_before_optimize() override;
    void hook_optimize() override;
public:
    explicit ColumnGeneration(const BlockModel<Ctr>& t_model);

    [[nodiscard]] const BlockModel<Ctr>& parent() const override;

    AbstractModel& master() { return *m_master; } // TODO make this private

    template<class T, class ...ArgsT> T& set_master_backend(ArgsT&& ...t_args) { return Idol::set_optimizer<T>(*m_master, std::forward<ArgsT>(t_args)...); }

    template<class T, class ...ArgsT> T& set_subproblem_backend(unsigned int t_index, ArgsT&& ...t_args) { return Idol::set_optimizer<T>(m_subproblems[t_index].model(), std::forward<ArgsT>(t_args)...); }

    using Algorithm::set;
    using Algorithm::get;

    // Variables
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    // Parameters
    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    void set(const Parameter<int>& t_param, int t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
    [[nodiscard]] int get(const Parameter<int>& t_param) const override;
};

#endif //IDOL_COLUMNGENERATION_H
