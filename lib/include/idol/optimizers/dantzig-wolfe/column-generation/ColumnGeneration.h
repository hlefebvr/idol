//
// Created by henri on 31.10.23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include "../Optimizers_DantzigWolfeDecomposition.h"

class idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration {
    DantzigWolfeDecomposition& m_parent;

    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    std::optional<Solution::Dual> m_last_master_solution;
    std::optional<Solution::Dual> m_stability_center;
    std::vector<DantzigWolfe::SubProblem::PhaseId> m_sub_problems_phases;
    double m_best_obj = -Inf;
    double m_best_bound = +Inf;

    bool m_use_farkas_for_infeasibility = true;
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
public:
    explicit ColumnGeneration(DantzigWolfeDecomposition& t_parent);

    void execute();
};

#endif //IDOL_COLUMNGENERATION_H
