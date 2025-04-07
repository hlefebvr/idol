//
// Created by henri on 07.04.25.
//

#ifndef IDOL_CPLEX_H
#define IDOL_CPLEX_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include <list>
#include <memory>
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/general/utils/Map.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

#ifdef IDOL_USE_CPLEX
#include <ilcplex/ilocplex.h>
#else
// enum GRB_IntParam {};
// enum GRB_DoubleParam {};
#endif

namespace idol {
    class Callback;
    class Cplex;
}

class idol::Cplex : public OptimizerFactoryWithDefaultParameters<Cplex> {
    std::optional<bool> m_continuous_relaxation;
    std::optional<unsigned int> m_max_n_solution_in_pool;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

public:
    Cplex() = default;

    Cplex(const Cplex& t_src);
    Cplex(Cplex&&) noexcept = default;

    Cplex& operator=(const Cplex&) = delete;
    Cplex& operator=(Cplex&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    static Cplex ContinuousRelaxation();

    Cplex& add_callback(const CallbackFactory& t_cb);

    Cplex& with_max_n_solution_in_pool(unsigned int t_value);

    Cplex& with_continuous_relaxation_only(bool t_value);

    static Model read_from_file(Env& t_env, const std::string& t_filename);

    [[nodiscard]] Cplex *clone() const override;
};

#endif //IDOL_CPLEX_H
