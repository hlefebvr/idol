//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ALTERNATINGDIRECTIONMETHOD_H
#define IDOL_ALTERNATINGDIRECTIONMETHOD_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/containers/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include <optional>

namespace idol {
    class AlternatingDirectionMethod;
}

class idol::AlternatingDirectionMethod : public OptimizerFactoryWithDefaultParameters<AlternatingDirectionMethod> {
public:
    explicit AlternatingDirectionMethod(Annotation<Var, unsigned int> t_decomposition);

    AlternatingDirectionMethod(const AlternatingDirectionMethod& t_src) = default;
    AlternatingDirectionMethod(AlternatingDirectionMethod&&) = default;

    AlternatingDirectionMethod& operator=(const AlternatingDirectionMethod&) = default;
    AlternatingDirectionMethod& operator=(AlternatingDirectionMethod&&) = default;

    AlternatingDirectionMethod& with_default_sub_problem_spec(AlternatingDirection::SubProblem t_sub_problem);

    AlternatingDirectionMethod& with_penalization(const Annotation<Ctr, bool>& t_penalized_constraints);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;

private:
    std::optional<Annotation<Var, unsigned int>> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;
    std::optional<AlternatingDirection::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, AlternatingDirection::SubProblem> m_sub_problem_specs;

    std::vector<AlternatingDirection::SubProblem> create_sub_problem_specs(const Model& t_model, const AlternatingDirection::Formulation& t_formulation) const;
};


#endif //IDOL_ALTERNATINGDIRECTIONMETHOD_H
