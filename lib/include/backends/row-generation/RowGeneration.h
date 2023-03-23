//
// Created by henri on 06/03/23.
//

#ifndef IDOL_ROWGENERATION_H
#define IDOL_ROWGENERATION_H

#include "backends/Algorithm.h"
#include "modeling/models/BlockModel.h"
#include "RowGenerationSP.h"

class RowGeneration;

IDOL_CREATE_PARAMETER_CLASS(RowGeneration);

IDOL_CREATE_PARAMETER_TYPE(RowGeneration, bool, 1);

IDOL_CREATE_PARAMETER(RowGeneration, bool, 0, BranchingOnDual, false);

class RowGeneration : public Algorithm {
    std::unique_ptr<AbstractModel> m_master;
    std::vector<RowGenerationSP> m_subproblems;

    int m_sense = Minimize;
    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_rows_at_last_iteration = 0;
    bool m_is_nested = false;

    std::optional<Solution::Primal> m_current_primal_solution;
protected:
    void hook_before_optimize() override;

    void hook_optimize() override;

    void initialize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    void main_loop(bool t_solve_master);
    void solve_master_problem();
    void analyze_master_problem_solution();
    void log_master_solution(bool t_force = false) const;
    [[nodiscard]] bool stopping_condition() const;
    void update_subproblems();
    void solve_subproblems();
    void log_subproblems_solution() const;
    void log_subproblem_solution(const RowGenerationSP& t_subproblem, bool t_force = false) const;
    void analyze_subproblems_solution();
    void clean_up();
    void enrich_master_problem();

    using Algorithm::set;
    using Algorithm::get;

    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override;
public:
    explicit RowGeneration(const BlockModel<Var>& t_model);

    [[nodiscard]] const BlockModel<Var>& parent() const override;

    AbstractModel& master() { return *m_master; } // TODO make this private

    [[nodiscard]] const AbstractModel& master() const { return *m_master; }

    [[nodiscard]] auto subproblems() const { return ConstIteratorForward<std::vector<RowGenerationSP>>(m_subproblems); }

    [[nodiscard]] const RowGenerationSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index]; }

};

#endif //IDOL_ROWGENERATION_H
