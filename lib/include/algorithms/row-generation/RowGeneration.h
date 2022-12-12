//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATION_H
#define OPTIMIZE_CUTGENERATION_H

#include "../decomposition/GenerationAlgorithm.h"
#include "RowGenerationSP.h"
#include <list>
#include <memory>

class RowGeneration : public GenerationAlgorithm {
protected:
    std::list<RowGenerationSP> m_subproblems;
    std::unique_ptr<Solution::Primal> m_last_rmp_primals;

    bool m_is_terminated = false;
    bool m_violated_cut_found_at_last_iteration = true;
    unsigned int m_iteration = 0;

    void initialize();
    void save_last_rmp_primal_solution();
    void analyze_last_rmp_primal_solution();
    [[nodiscard]] bool rmp_is_unbounded() const;
    [[nodiscard]] bool rmp_is_infeasible() const;
    [[nodiscard]] bool rmp_could_not_be_solved_to_optimality() const;
    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void update_subproblems();
    void solve_subproblems();
    virtual void solve_rmp();
    void log_last_rmp_primal_solution() const;
    void analyze_last_subproblem_primal_solution(const RowGenerationSP& t_subproblem);
    void add_cuts();
    [[nodiscard]] bool iteration_limit_is_reached() const;
    [[nodiscard]] bool time_limit_is_reached() const;

    void terminate();
    void terminate_for_rmp_is_infeasible();
    void terminate_for_rmp_could_not_be_solved_to_optimality();
    void terminate_for_no_violated_cut_found();
    void terminate_for_subproblem_is_infeasible();
    void terminate_for_subproblem_could_not_be_solved_to_optimality();
    void terminate_for_iteration_limit_is_reached();
    void terminate_for_time_limit_is_reached();

    void execute() override;
public:
    explicit RowGeneration(Algorithm& t_rmp_solution_strategy);

    [[nodiscard]] Solution::Primal primal_solution() const override;

    RowGenerationSP& add_subproblem(const Ctr& t_cut);

    using Subproblems = IteratorForward<std::list<RowGenerationSP>>;
    Subproblems subproblems() { return Subproblems(m_subproblems);}
};

#endif //OPTIMIZE_CUTGENERATION_H
