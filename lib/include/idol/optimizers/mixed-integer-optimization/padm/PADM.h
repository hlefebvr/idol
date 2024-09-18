//
// Created by henri on 18.09.24.
//

#ifndef IDOL_PADM_H
#define IDOL_PADM_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/containers/Map.h"
#include <optional>

namespace idol {
    class PADM;
}

class idol::PADM : public OptimizerFactoryWithDefaultParameters<PADM> {
public:
    explicit PADM(Annotation<Var, unsigned int> t_decomposition);

    PADM(Annotation<Var, unsigned int> t_decomposition, Annotation<Ctr, bool> t_penalized_constraints);

    PADM(const PADM& t_src) = default;
    PADM(PADM&&) = default;

    PADM& operator=(const PADM&) = default;
    PADM& operator=(PADM&&) = default;

    class SubProblem {
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    public:
        SubProblem() = default;

        SubProblem(const SubProblem&);
        SubProblem(SubProblem&&) = default;

        SubProblem& operator=(const SubProblem&) = delete;
        SubProblem& operator=(SubProblem&&) = default;

        SubProblem& with_optimizer(const OptimizerFactory& t_optimizer_factory);
    };

    PADM& with_default_sub_problem_spec(SubProblem t_sub_problem);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;

private:
    std::optional<Annotation<Var, unsigned int>> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;
    std::optional<SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, SubProblem> m_sub_problem_specs;
};


#endif //IDOL_PADM_H
