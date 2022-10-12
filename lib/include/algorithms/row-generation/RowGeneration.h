//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATION_H
#define OPTIMIZE_CUTGENERATION_H

#include "../decomposition/GenerationAlgorithmWithAttributes.h"
#include "../attributes/Attributes_Base.h"
#include "../attributes/Attributes.h"
#include "RowGenerationSP.h"
#include <list>
#include <memory>

class RowGeneration : public GenerationAlgorithmWithAttributes<AttributesSections::Base> {
    std::list<RowGenerationSP> m_subproblems;
    std::unique_ptr<Solution::Primal> m_last_rmp_primals;

    bool m_is_terminated = false;

    void initialize();
    void save_last_rmp_primal_solution();
    void analyze_last_rmp_primal_solution();
    [[nodiscard]] bool rmp_is_unbounded() const;
    [[nodiscard]] bool rmp_is_infeasible() const;
    [[nodiscard]] bool rmp_could_not_be_solved_to_optimality() const;
    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void update_subproblems();
    void solve_subproblems();
    void log_last_rmp_primal_solution() const;
    void analyze_last_subproblem_primal_solution(const RowGenerationSP& t_subproblem);
    void add_cuts();

    void terminate();
    void terminate_for_rmp_is_infeasible();
    void terminate_for_rmp_could_not_be_solved_to_optimality();
    void terminate_for_no_violated_cut_found();
    void terminate_for_subproblem_is_infeasible();
    void terminate_for_subproblem_could_not_be_solved_to_optimality();
protected:
    void execute() override;
public:

    explicit RowGeneration(DecompositionId&& t_id);

    void build() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    void update_lb(const Var &t_var, double t_lb) override;

    void update_ub(const Var &t_var, double t_ub) override;

    RowGenerationSP& add_subproblem(const Ctr& t_cut);
};

#endif //OPTIMIZE_CUTGENERATION_H
