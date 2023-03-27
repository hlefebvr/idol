//
// Created by henri on 07/02/23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include "../Algorithm.h"
#include "modeling/models/BlockModel.h"
#include "Parameters_ColumnGeneration.h"
#include "ColumnGenerationSP.h"

namespace Backends {
    class ColumnGeneration;
}

class Backends::ColumnGeneration : public Algorithm {
    std::unique_ptr<OptimizerFactory> m_pricing_optimizer;

    std::unique_ptr<AbstractModel> m_master;
    std::vector<ColumnGenerationSP> m_subproblems;

    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;

    std::list<Var> m_artificial_variables;
    bool m_current_is_farkas_pricing = false;
    bool m_current_is_phase_I = false;

    std::optional<Solution::Dual> m_current_dual_solution;
    std::optional<Solution::Dual> m_adjusted_dual_solution;

    Param::ColumnGeneration::values<bool> m_bool_parameters;
    Param::ColumnGeneration::values<int> m_int_parameters;
    Param::ColumnGeneration::values<double> m_double_parameters;
protected:
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    virtual void add_artificial_variables();
    virtual void switch_to_pure_phase_I();
    virtual void restore_from_pure_phase_I();
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

    using Algorithm::set;
    using Algorithm::get;

    // Model
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_expr) override;
    // Variables
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override;
    // Parameters
    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    void set(const Parameter<int>& t_param, int t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
    [[nodiscard]] int get(const Parameter<int>& t_param) const override;
public:
    explicit ColumnGeneration(const BlockModel<Ctr>& t_model);

    ColumnGeneration(const BlockModel<Ctr>& t_model,
                     const OptimizerFactory& t_master_optimizer,
                     const OptimizerFactory& t_pricing_optimizer);

    [[nodiscard]] const BlockModel<Ctr>& parent() const override;

    AbstractModel& master() { return *m_master; } // TODO make this private

    [[nodiscard]] const AbstractModel& master() const { return *m_master; }

    [[nodiscard]] auto subproblems() const { return ConstIteratorForward<std::vector<ColumnGenerationSP>>(m_subproblems); }

    [[nodiscard]] const ColumnGenerationSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index]; }

};

#endif //IDOL_COLUMNGENERATION_H
