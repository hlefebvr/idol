//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATION_H
#define OPTIMIZE_COLUMNGENERATION_H

#include "../decomposition/GenerationAlgorithm.h"
#include "../../modeling/models/Model.h"
#include "ColumnGenerationSP.h"

class ColumnGeneration : public GenerationAlgorithm {
    std::list<ColumnGenerationSP> m_subproblems;
    std::unique_ptr<Solution::Dual> m_last_rmp_duals;

    bool m_is_terminated = false;

    void initialize();
    void save_last_rmp_dual_solution();
    void analyze_last_rmp_dual_solution();
    [[nodiscard]] bool rmp_is_unbounded() const;
    [[nodiscard]] bool rmp_is_infeasible() const;
    [[nodiscard]] bool rmp_could_not_be_solved_to_optimality() const;
    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void save_rmp_farkas();
    void update_subproblems();
    void solve_subproblems();
    void log_last_rmp_dual_solution() const;
    void analyze_last_subproblem_primal_solution(const ColumnGenerationSP& t_subproblem);
    void add_columns();

    void terminate();
    void terminate_for_rmp_is_unbounded();
    void terminate_for_rmp_could_not_be_solved_to_optimality();
    void terminate_for_subproblem_is_infeasible();
    void terminate_for_subproblem_is_unbounded();
    void terminate_for_subproblem_could_not_be_solved_to_optimality();
    void terminate_for_no_improving_column_found();
protected:
    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;

    void execute() override;
public:
    explicit ColumnGeneration(Algorithm& t_rmp_solution_strategy);

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;

    void remove(const Ctr &t_constraint) override;

    ColumnGenerationSP& add_subproblem(const Var& t_var);

    using Subproblems = IteratorForward<std::list<ColumnGenerationSP>>;
    using ConstSubproblems = ConstIteratorForward<std::list<ColumnGenerationSP>>;

    Subproblems subproblems() { return m_subproblems; }
    [[nodiscard]] ConstSubproblems subproblems() const { return m_subproblems; }

    void set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) override;
};

#endif //OPTIMIZE_COLUMNGENERATION_H
