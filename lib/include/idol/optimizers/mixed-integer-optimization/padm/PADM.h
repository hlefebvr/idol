//
// Created by henri on 18.09.24.
//

#ifndef IDOL_PADM_H
#define IDOL_PADM_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/containers/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include <optional>

namespace idol {
    class PADM;
}

class idol::PADM : public OptimizerFactoryWithDefaultParameters<PADM> {
public:
    explicit PADM(Annotation<Var, unsigned int> t_decomposition);

    PADM(const PADM& t_src) = default;
    PADM(PADM&&) = default;

    PADM& operator=(const PADM&) = default;
    PADM& operator=(PADM&&) = default;

    PADM& with_default_sub_problem_spec(ADM::SubProblem t_sub_problem);

    PADM& with_penalization(const Annotation<Ctr, bool>& t_penalized_constraints);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;

private:
    std::optional<Annotation<Var, unsigned int>> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;
    std::optional<ADM::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, ADM::SubProblem> m_sub_problem_specs;

    std::vector<ADM::SubProblem> create_sub_problem_specs(const Model& t_model, const ADM::Formulation& t_formulation) const;
};


#endif //IDOL_PADM_H
