//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONSP_H
#define OPTIMIZE_COLUMNGENERATIONSP_H

#include "../Algorithm.h"
#include "../decomposition/Generator.h"
#include "ColumnGenerationBranchingSchemes_SP.h"
#include "../logs/Log.h"
#include "../../containers/Set.h"
#include <memory>
#include <list>
#include <iomanip>
#include <functional>

class Model;

class ColumnGenerationSP {

    TempVar m_var_template;

    Algorithm& m_rmp_strategy;
    std::unique_ptr<Algorithm> m_exact_solution_strategy;
    std::unique_ptr<ColumnGenerationBranchingScheme> m_branching_scheme;
    std::list<std::unique_ptr<Solution::Primal>> m_primal_solutions;
    Set<unsigned int> m_subproblem_ids;

    using PresentColumnList = std::list<std::pair<Var, Solution::Primal&>>;

    PresentColumnList m_currently_present_variables;

    void remove_var_template_from_rmp(const Var& t_var);
public:
    explicit ColumnGenerationSP(Algorithm& t_rmp_strategy, const Var& t_var);

    void initialize();

    void solve();

    using PresentColumns = ConstIteratorForward<PresentColumnList>;

    [[nodiscard]] PresentColumns currently_present_variables() const { return PresentColumns(m_currently_present_variables); }

    [[nodiscard]] Solution::Primal primal_solution() const;

    [[nodiscard]] Solution::Dual dual_solution() const;

    [[nodiscard]] Expr<Var, Var> get_pricing_objective(const Solution::Dual &t_duals) const;

    void update_pricing_objective(const Expr<Var, Var>& t_objective);

    void log_last_primal_solution() const;

    void save_last_primal_solution();

    [[nodiscard]] bool is_unbounded() const;

    [[nodiscard]] bool is_infeasible() const;

    [[nodiscard]] bool could_not_be_solved_to_optimality() const;

    [[nodiscard]] bool improving_column_found() const;

    [[nodiscard]] TempVar create_column_from(const Solution::Primal& t_primal_solution) const;

    void add_column_to_rmp();

    bool set_lower_bound(const Var &t_var, double t_lb);

    bool set_upper_bound(const Var &t_var, double t_ub);

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }

    const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    Algorithm& rmp_solution_strategy() { return m_rmp_strategy; }

    void remove_column_if(const std::function<bool(const Var&, const Solution::Primal&)>& t_indicator_for_removal);

    std::optional<Ctr> add_constraint(TempCtr &t_temporay_constraint);

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs);

    bool remove_constraint(const Ctr& t_ctr);

    void remove_columns_violating_lower_bound(const Var& t_var, double t_lb);

    void remove_columns_violating_upper_bound(const Var& t_var, double t_ub);

    void remove_columns_violating_constraint(const TempCtr& t_ctr);

    LinExpr<Var> expand(const Var &t_subproblem_variable) const;

    void reset_linking_expr(const Ctr& t_ctr);

    void add_linking_expr(const Ctr& t_ctr, const LinExpr<Var>& t_expr);

    template<class T, class ...Args> T& set_solution_strategy(Args&& ...t_args);

    template<class T, class ...Args> T& set_branching_scheme(Args&& ...t_args);
};

template<class T, class... Args>
T &ColumnGenerationSP::set_solution_strategy(Args &&... t_args) {
    auto* exact_solution_strategy = new T(std::forward<Args>(t_args)...);
    m_exact_solution_strategy.reset(exact_solution_strategy);
    return *exact_solution_strategy;
}

template<class T, class... Args>
T &ColumnGenerationSP::set_branching_scheme(Args &&... t_args) {
    auto* branching = new T(std::forward<Args>(t_args)...);
    m_branching_scheme.reset(branching);
    return *branching;
}

#endif //OPTIMIZE_COLUMNGENERATIONSP_H
