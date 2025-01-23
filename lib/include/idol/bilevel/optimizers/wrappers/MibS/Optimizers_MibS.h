//
// Created by henri on 01.02.24.
//

#ifndef IDOL_OPTIMIZERS_MIBS_H
#define IDOL_OPTIMIZERS_MIBS_H

#ifdef IDOL_USE_MIBS

#include <OsiSolverInterface.hpp>
#include "idol/general/optimizers/Optimizer.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/bilevel/modeling/Description.h"
#include "impl_MibS.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"

namespace idol::Optimizers::Bilevel {
    class MibS;
}

class idol::Optimizers::Bilevel::MibS : public Optimizer {
    const idol::Bilevel::Description m_description;

    std::unique_ptr<idol::impl::MibS> m_mibs;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;
    const bool m_use_file;
    const bool m_use_cplex_for_feasibility;

    std::list<std::unique_ptr<Callback>> m_callbacks;
public:
    MibS(const idol::Model& t_parent,
         idol::Bilevel::Description  t_description,
         OsiSolverInterface* t_osi_solver,
         bool t_use_file,
         bool t_use_cplex_for_feasibility);

    [[nodiscard]] std::string name() const override { return "mibs"; }
    void throw_if_no_mibs() const;

    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;
    void add_callback(Callback* t_callback);

    [[nodiscard]] const idol::Bilevel::Description& bilevel_description() const { return m_description; }

protected:
    void build() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void add(const QCtr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void remove(const QCtr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;
    void hook_optimize() override;
    void set_solution_index(unsigned int t_index) override;
    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;
    void update_ctr_type(const Ctr &t_ctr) override;
    void update_ctr_rhs(const Ctr &t_ctr) override;
    void update_var_type(const Var &t_var) override;
    void update_var_lb(const Var &t_var) override;
    void update_var_ub(const Var &t_var) override;
    void update_var_obj(const Var &t_var) override;
};

#endif

#endif //IDOL_OPTIMIZERS_MIBS_H
