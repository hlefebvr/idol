//
// Created by henri on 10/09/22.
//

#ifndef OPTIMIZE_LPSOLVE_H
#define OPTIMIZE_LPSOLVE_H
#ifdef USE_LPSOLVE

#include "solvers/BaseSolver.h"
#include "containers/Optional.h"
#include <stack>

struct _lprec; // NOLINT(bugprone-reserved-identifier)

class Lpsolve final : public BaseSolver<int, int> {
    _lprec* model = nullptr;
    double m_objective_offset = 0.;
    Optional<SolutionStatus> m_solution_status;
    Optional<Solution::Primal> m_ray;
    Optional<Solution::Dual> m_farkas;
    bool m_infeasible_or_unbounded_info = false;

    std::stack<int> m_free_columns;
    std::stack<int> m_free_constraints;

    static void throw_if_error(unsigned char t_code, const std::string& t_msg);

    static void throw_if_error(void* t_ptr, const std::string& t_msg);
public:
    explicit Lpsolve(Model& t_model);

    ~Lpsolve() override;

    void write(const std::string &t_filename) override;

    void solve() override;

protected:
    int create_variable(const Var &t_var) override;

    int create_constraint(const Ctr &t_ctr) override;

    void fill_column(const Var &t_var) override;

    void fill_row(const Ctr &t_ctr) override;

    void remove_variable(const Var &t_var) override;

    void remove_constraint(const Ctr &t_ctr) override;

    void set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) override;

    void set_objective_offset(const Coefficient &t_offset) override;

    void set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) override;

    void set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) override;

    void set_lb(const Var &t_var, double t_lb) override;

    void set_ub(const Var &t_var, double t_ub) override;

    void set_type(const Var &t_var, VarType t_type) override;

    void set_type(const Ctr &t_ctr, CtrType t_type) override;

    [[nodiscard]] SolutionStatus get_primal_status() const override;

    [[nodiscard]] double get_primal_objective_value() const override;

    [[nodiscard]] double get_primal_value(const Var &t_var) const override;

    [[nodiscard]] double get_unbounded_ray(const Var &t_var) const override;

    double get_unbounded_ray_objective_value() const override;

    [[nodiscard]] double get_dual_value(const Ctr &t_ctr) const override;

    [[nodiscard]] double get_dual_farkas_objective_value() const override;

    [[nodiscard]] double get_dual_farkas_value(const Ctr &t_ctr) const override;

    void compute_unbounded_ray();

    void compute_farkas_dual();

public:
    void set_presolve(bool t_value) override;

    bool presolve() const override;

    Solution::Primal unbounded_ray() const override;

    Solution::Dual dual_farkas() const override;

    [[nodiscard]] bool infeasible_or_unbounded_info() const override;

    void set_infeasible_or_unbounded_info(bool t_value) override;

    void set_algorithm_for_lp(AlgorithmForLP t_algorithm) override;

    AlgorithmForLP algorithm_for_lp() const override;
};

#endif
#endif //OPTIMIZE_LPSOLVE_H