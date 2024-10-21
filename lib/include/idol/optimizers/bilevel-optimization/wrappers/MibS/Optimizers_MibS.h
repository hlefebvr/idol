//
// Created by henri on 01.02.24.
//

#ifndef IDOL_OPTIMIZERS_MIBS_H
#define IDOL_OPTIMIZERS_MIBS_H

#ifdef IDOL_USE_MIBS

#include <OsiSolverInterface.hpp>
#include "idol/optimizers/Optimizer.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "impl_MibS.h"

namespace idol::Optimizers::Bilevel {
    class MibS;
}

class idol::Optimizers::Bilevel::MibS : public Optimizer {
    const idol::Bilevel::LowerLevelDescription m_description;

    std::unique_ptr<idol::impl::MibS> m_mibs;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;
    const bool m_use_file;
public:
    MibS(const idol::Model& t_parent,
         idol::Bilevel::LowerLevelDescription  t_description,
         OsiSolverInterface* t_osi_solver,
         bool t_use_file);

    std::string name() const override { return "mibs"; }
    void throw_if_no_mibs() const;

    SolutionStatus get_status() const override;
    SolutionReason get_reason() const override;
    double get_best_obj() const override;
    double get_best_bound() const override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_reduced_cost(const Var &t_var) const override;
    double get_var_ray(const Var &t_var) const override;
    double get_ctr_dual(const Ctr &t_ctr) const override;
    double get_ctr_farkas(const Ctr &t_ctr) const override;
    double get_relative_gap() const override;
    double get_absolute_gap() const override;
    unsigned int get_n_solutions() const override;
    unsigned int get_solution_index() const override;

protected:
    void build() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
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
