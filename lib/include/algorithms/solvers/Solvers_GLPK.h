//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVERS_GLPK_H
#define OPTIMIZE_SOLVERS_GLPK_H

#include "Solver.h"
#include "../../containers/Optional.h"
#include <stack>

#ifdef IDOL_USE_GLPK

struct glp_prob;

namespace Solvers {
    class GLPK;
}

class Solvers::GLPK : public Solver<int, int> {

    glp_prob* m_model;
    unsigned int n_solved = 0;
    bool m_solved_as_mip = false;

    Optional<Solution::Primal> m_ray;
    Optional<Solution::Dual> m_farkas;

    bool m_rebuild_basis = false;
    std::stack<int> m_deleted_variables;
    std::stack<int> m_deleted_constraints;
protected:
    void execute() override;

    int create(const Var& t_var, bool t_with_collaterals) override;
    int create(const Ctr& t_ctr, bool t_with_collaterals) override;

    void update(const Var& t_var, int& t_impl) override;
    void update(const Ctr& t_ctr, int& t_impl) override;

    void update_obj() override;
    void update_rhs() override;
    void clear_rhs() override;

    void remove(const Var &t_var, int &t_impl) override;
    void remove(const Ctr &t_ctr, int &t_impl) override;

    void save_simplex_solution_status();
    void save_milp_solution_status();
    void compute_farkas_certificate();
    void compute_unbounded_ray();
    [[nodiscard]] double objective_value() const;
public:
    explicit GLPK(Model& t_model);

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Solution::Dual farkas_certificate() const override;

    [[nodiscard]] Solution::Primal unbounded_ray() const override;

    using Solver<int, int>::update;

    glp_prob* impl() { return m_model; }

    [[nodiscard]] const glp_prob* impl() const { return m_model; }

    using Algorithm::remove;

    void write(const std::string &t_filename) override;

    using Solver::set;
    using Solver::get;

    void set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) override;
    void set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override;
    void set(const Req<int, Var>& t_attr, const Var& t_var, int t_value) override;

    [[nodiscard]] double get(const Req<double, void>& t_attr) const override;
};

#endif

#endif //OPTIMIZE_SOLVERS_GLPK_H
