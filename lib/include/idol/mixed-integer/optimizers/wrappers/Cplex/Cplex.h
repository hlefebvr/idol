//
// Created by Henri on 12/06/2026.
//

#ifndef IDOL_CPLEX_H
#define IDOL_CPLEX_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include <list>
#include <memory>
#include "idol/general/utils/Map.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

namespace idol {
    class Callback;
    class Cplex;
}

class idol::Cplex : public OptimizerFactoryWithDefaultParameters<Cplex> {
    std::optional<bool> m_continuous_relaxation;
    std::optional<unsigned int> m_max_n_solution_in_pool;

    Map<int, int>    m_int_params;
    Map<int, double> m_double_params;
protected:
    [[nodiscard]] Optimizer *create(const Model &t_model) const override;
public:
    Cplex() = default;

    Cplex(const Cplex& t_src);
    Cplex(Cplex&&) noexcept = default;

    Cplex& operator=(const Cplex&) = delete;
    Cplex& operator=(Cplex&&) noexcept = default;

    static Cplex ContinuousRelaxation();

    Cplex& with_max_n_solution_in_pool(unsigned int t_value);
    Cplex& with_continuous_relaxation_only(bool t_value);
    Cplex& with_external_param(int t_param, int t_value);
    Cplex& with_external_param(int t_param, double t_value);

    [[nodiscard]] Cplex *clone() const override;
};

#endif //IDOL_CPLEX_H