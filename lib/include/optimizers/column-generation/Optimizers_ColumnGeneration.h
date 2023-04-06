//
// Created by henri on 24/03/23.
//

#ifndef IDOL_OPTIMIZERS_COLUMNGENERATION_H
#define IDOL_OPTIMIZERS_COLUMNGENERATION_H

#include "../Algorithm.h"
#include "modeling/models/Model.h"
#include "containers/GeneratorPool.h"

namespace Optimizers {
    class ColumnGeneration;
    class DantzigWolfeDecomposition;
}

class Optimizers::ColumnGeneration : public Algorithm {
public:
    class Subproblem;

    ColumnGeneration(const Model& t_model,
                     Model *t_master_problem,
                     const std::vector<Model *> &t_subproblems,
                     std::vector<Column> t_generation_patterns);

    [[nodiscard]] std::string name() const override { return "column-generation"; }

    [[nodiscard]] const Model& master() const { return *m_master; }

    [[nodiscard]] auto subproblems() const { return ConstIteratorForward(m_subproblems); }

    virtual void set_parallel_pricing_limit(unsigned int t_limit) { m_parallel_pricing_limit = t_limit; }

    [[nodiscard]] unsigned int parallel_pricing_limit() const { return m_parallel_pricing_limit; }

    virtual void set_clean_up_ratio(double t_ratio) { m_clean_up_ratio = t_ratio; }

    [[nodiscard]] double clean_up_ratio() const { return m_clean_up_ratio; }

    virtual void set_clean_up_threshold(unsigned int t_threshold) { m_clean_up_threshold = t_threshold; }

    [[nodiscard]] unsigned int clean_up_threshold() const { return m_clean_up_threshold; }

    virtual void set_farkas_pricing(bool t_value) { m_farkas_pricing = t_value; }

    [[nodiscard]] bool farkas_pricing() const { return m_farkas_pricing; }

    virtual void set_artificial_variables_cost(double t_cost) { m_artificial_variables_cost = t_cost; }

    [[nodiscard]] double artificial_variable_cost() const { return m_artificial_variables_cost; }

    virtual void set_dual_price_smoothing_stabilization_factor(double t_factor) { m_smoothing_factor = t_factor; }

    [[nodiscard]] double dual_price_smoothing_stabilization_factor() const { return m_smoothing_factor; }

    virtual void set_log_frequency(unsigned int t_frequency) { m_log_frequency = t_frequency; }

    [[nodiscard]] unsigned int log_frequency() const { return m_log_frequency; }

    Subproblem& add_subproblem(Model* t_sub_problem_model, Column t_generation_pattern);

    [[nodiscard]] unsigned int n_generated_columns() const { return m_n_generated_columns; }
protected:
    void hook_before_optimize() override;
    void hook_optimize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    virtual void run_column_generation();
    virtual void add_artificial_variables();
    [[nodiscard]] virtual bool has_artificial_variable_in_basis() const;
    virtual void solve_master_problem();
    void log_master_solution(bool t_force = false) const;
    void log_subproblem_solution(const Subproblem& t_subproblem, bool t_force = false) const;
    virtual void analyze_master_problem_solution();
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution();
    virtual void enrich_master_problem();
    virtual void clean_up();
    virtual void remove_artificial_variables();
    [[nodiscard]] virtual bool stopping_condition() const;
    void terminate_for_master_infeasible_with_artificial_variables();
    void set_phase_I_objective_function();
    void restore_objective_function();

    virtual void set_objective(Expr<Var, Var>&& t_objective);
    virtual void set_objective(const Expr<Var, Var>& t_objective);

    [[nodiscard]] double get_var_val(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_val(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;

    void update_obj_sense() override;

    void update_obj() override;

    void update_rhs() override;

    void update_obj_constant() override;

    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;

    void update_ctr_type(const Ctr &t_ctr) override;

    void update_ctr_rhs(const Ctr &t_ctr) override;

    void update_var_type(const Var &t_var) override;

    void update_var_lb(const Var &t_var) override;

    void update_var_ub(const Var &t_var) override;

    void update_var_obj(const Var &t_var) override;

    std::unique_ptr<Model> m_master;
    std::vector<Subproblem> m_subproblems;

    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;
    unsigned int m_n_generated_columns = 0;

    std::list<Var> m_artificial_variables;
    bool m_current_iteration_is_farkas_pricing = false;

    std::optional<Solution::Dual> m_current_dual_solution;
    std::optional<Solution::Dual> m_adjusted_dual_solution;

    unsigned int m_parallel_pricing_limit = 1;
    double m_clean_up_ratio = .75;
    unsigned int m_clean_up_threshold = 1e4;
    bool m_farkas_pricing = true;
    double m_artificial_variables_cost = -1;
    double m_smoothing_factor = 0.;
    unsigned int m_log_frequency = 10;
};

class Optimizers::ColumnGeneration::Subproblem {
    friend class ::Optimizers::ColumnGeneration;
    friend class ::Optimizers::DantzigWolfeDecomposition;

    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;

    unsigned int m_index;
    ColumnGeneration& m_parent;
    std::unique_ptr<Model> m_model;
    Column m_generation_pattern;

    GeneratorPool<Var> m_pool;
    PresentGeneratorsList m_present_generators;

    void hook_before_optimize();

    void update_objective(bool t_farkas_pricing, const Solution::Dual& t_duals);

    void optimize();

    [[nodiscard]] double compute_reduced_cost(const Solution::Dual& t_duals) const;

    void enrich_master_problem();

    [[nodiscard]] TempVar create_column_from_generator(const Solution::Primal& t_primals) const;

    void clean_up();

    void remove_column_if(const std::function<bool(const Var&, const Solution::Primal&)>& t_indicator_for_removal);

    void update_generation_pattern_objective(Constant&& t_objective);
public:
    Subproblem(ColumnGeneration& t_parent, unsigned int t_index, Model* t_model, Column&& t_generation_pattern);

    [[nodiscard]] auto present_generators() const { return ConstIteratorForward(m_present_generators); }

    [[nodiscard]] const auto& pool() const { return m_pool; }

    [[nodiscard]] const auto& model() const { return *m_model; }
};

#endif //IDOL_OPTIMIZERS_COLUMNGENERATION_H
