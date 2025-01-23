//
// Created by henri on 31.10.23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include "Optimizers_DantzigWolfeDecomposition.h"

class idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration {
    DantzigWolfeDecomposition& m_parent;
    double m_best_bound_stop;

    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    std::optional<PrimalPoint> m_master_primal_solution;
    std::optional<DualPoint> m_last_master_solution;
    std::vector<DantzigWolfe::SubProblem::PhaseId> m_sub_problems_phases;
    double m_best_obj = -Inf;
    double m_best_bound = +Inf;

    const bool m_use_farkas_for_infeasibility;
    unsigned int m_iteration_count = 0;
    unsigned int m_n_generated_columns = 0;
    bool m_solve_dual_master = false;
    bool m_is_terminated = false;
    bool m_current_iteration_is_using_farkas = false;

    void initialize_sub_problem_phases();
    void solve_dual_master();
    bool gap_is_closed() const;
    bool check_stopping_criterion();
    void update_sub_problems();
    void solve_sub_problems_in_parallel();
    void analyze_sub_problems();
    void enrich_master();
    void pool_clean_up();

    void log_init();
    void log_master();
    void log_sub_problems();
    void log_end();
public:
    ColumnGeneration(DantzigWolfeDecomposition& t_parent, bool t_use_farkas_for_infeasibility, double t_best_bound_stop);

    const DantzigWolfeDecomposition& parent() const { return m_parent; }

    DantzigWolfeDecomposition& parent() { return m_parent; }

    SolutionStatus status() const { return m_status; }

    SolutionReason reason() const { return m_reason; }

    double best_obj() const { return m_best_obj; }

    double best_bound() const { return m_best_bound; }

    const PrimalPoint& primal_solution() const { return m_master_primal_solution.value(); }

    void set_best_bound_stop(double t_best_bound_stop) { m_best_bound_stop = t_best_bound_stop; }

    void execute();
};

#endif //IDOL_COLUMNGENERATION_H
