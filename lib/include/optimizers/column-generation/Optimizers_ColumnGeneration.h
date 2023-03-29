//
// Created by henri on 24/03/23.
//

#ifndef IDOL_OPTIMIZERS_COLUMNGENERATION_H
#define IDOL_OPTIMIZERS_COLUMNGENERATION_H

#include "../Algorithm.h"
#include "ColumnGeneration.h"
#include "optimizers/column-generation/Parameters_ColumnGeneration.h"
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
protected:
    void hook_before_optimize() override;
    void hook_optimize() override;
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
    void log_subproblem_solution(const Subproblem& t_subproblem, bool t_force = false) const;
    virtual void analyze_master_problem_solution();
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution();
    virtual void enrich_master_problem();
    virtual void clean_up();
    virtual void analyze_feasibility_with_artificial_variables();
    virtual void remove_artificial_variables();
    [[nodiscard]] virtual bool stopping_condition() const;

    using Algorithm::set;
    using Algorithm::get;

    // Variables
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override { return m_master->get(t_attr, t_var); }
    [[nodiscard]] int get(const Req<int, Var>& t_attr, const Var& t_var) const override { return m_master->get(t_attr, t_var); }
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override { m_master->set(t_attr, t_var, t_value); }
    void set(const Req<int, Var>& t_attr, const Var& t_var, int t_value) override { m_master->set(t_attr, t_var, t_value); }
    // Parameters
    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    void set(const Parameter<int>& t_param, int t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
    [[nodiscard]] int get(const Parameter<int>& t_param) const override;

    std::unique_ptr<Model> m_master;
    std::vector<Subproblem> m_subproblems;

    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns_at_last_iteration = 0;

    std::list<Var> m_artificial_variables;
    bool m_current_is_farkas_pricing = false;
    bool m_current_is_pure_phase_I = false;

    std::optional<Solution::Dual> m_current_dual_solution;
    std::optional<Solution::Dual> m_adjusted_dual_solution;

    Param::ColumnGeneration::values<bool> m_bool_parameters;
    Param::ColumnGeneration::values<int> m_int_parameters;
    Param::ColumnGeneration::values<double> m_double_parameters;
};

class Optimizers::ColumnGeneration::Subproblem {
    friend class ::Optimizers::ColumnGeneration;
    friend class ::Optimizers::DantzigWolfeDecomposition;

    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;
    using PresentGenerators = ConstIteratorForward<PresentGeneratorsList>;

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
};

#endif //IDOL_OPTIMIZERS_COLUMNGENERATION_H
