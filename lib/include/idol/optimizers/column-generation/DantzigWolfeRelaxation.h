//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFERELAXATION_H
#define IDOL_DANTZIGWOLFERELAXATION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "DantzigWolfeSubProblem.h"
#include "idol/containers/Map.h"
#include "DantzigWolfeFormulation.h"

namespace idol::DantzigWolfe {
    class Relaxation;
}

class idol::DantzigWolfe::Relaxation : public OptimizerFactoryWithDefaultParameters<Relaxation> {
    Annotation<Ctr, unsigned int> m_decomposition;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::optional<unsigned int> m_max_parallel_pricing;
    std::optional<DantzigWolfe::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, DantzigWolfe::SubProblem> m_sub_problem_specs;

    Relaxation(const Relaxation& t_src);
    void add_aggregation_constraints(Formulation& t_dantzig_wolfe_formulation) const;
public:
    explicit Relaxation(Annotation<Ctr, unsigned int> t_decomposition);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;

    Relaxation& with_master_optimizer(const OptimizerFactory& t_optimizer_factory);

    Relaxation& with_default_sub_problem_spec(SubProblem t_sub_problem);

    Relaxation& with_sub_problem_spec(unsigned int t_id, SubProblem t_sub_problem);

    const SubProblem& get_sub_problem_spec(unsigned int t_id) const;
};

#endif //IDOL_DANTZIGWOLFERELAXATION_H
