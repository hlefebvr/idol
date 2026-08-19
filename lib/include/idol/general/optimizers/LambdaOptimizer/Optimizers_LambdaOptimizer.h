//
// Created by charlotte on 05.06.26.
//

#ifndef IDOL_OPTIMIZERS_LAMBDA_OPTIMIZER_H
#define IDOL_OPTIMIZERS_LAMBDA_OPTIMIZER_H

#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"

namespace idol {
    namespace Optimizers {
        class LambdaOptimizer;
    }

    class LambdaContext;
}

class idol::Optimizers::LambdaOptimizer : public Algorithm {
    const std::function<void(LambdaContext&)>& m_lambda;
    std::vector<double> m_solution;

public:
    LambdaOptimizer(const Model& t_model,
        const std::function<void(LambdaContext&)>& t_lambda);

    [[nodiscard]] std::string name() const override;

    void set_var_primal(const Var &t_var, double t_value);

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;

    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;

    [[nodiscard]] double get_var_ray(const Var &t_var) const override;

    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;

    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;

    [[nodiscard]] unsigned int get_n_solutions() const override;

    [[nodiscard]] unsigned int get_solution_index() const override;

protected:
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

    void clear_solution();

    friend class idol::LambdaContext;
};

#endif //IDOL_OPTIMIZERS_LAMBDA_OPTIMIZER_H
