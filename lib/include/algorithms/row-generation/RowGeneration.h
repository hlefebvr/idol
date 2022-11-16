//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATION_H
#define OPTIMIZE_CUTGENERATION_H

#include "../decomposition/GenerationAlgorithmWithAttributes.h"
#include "../attributes/Attributes_Base.h"
#include "../attributes/Attributes.h"
#include "RowGenerationSP.h"
#include "Attributes_RowGeneration.h"
#include <list>
#include <memory>

class RowGeneration : public GenerationAlgorithmWithAttributes<AttributesSections::Base, AttributesSections::RowGeneration> {
protected:
    std::list<RowGenerationSP> m_subproblems;
    std::unique_ptr<Solution::Primal> m_last_rmp_primals;

    bool m_is_terminated = false;
    bool m_violated_cut_found_at_last_iteration = true;
    bool m_rmp_solved_to_optimality = true;
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

    void update_var_lb(const Var &t_var, double t_lb) override;

    void update_var_ub(const Var &t_var, double t_ub) override;

    RowGenerationSP& add_subproblem(const Ctr& t_cut);

    using Subproblems = IteratorForward<std::list<RowGenerationSP>>;
    Subproblems subproblems() { return Subproblems(m_subproblems);}
};

#endif //OPTIMIZE_CUTGENERATION_H
