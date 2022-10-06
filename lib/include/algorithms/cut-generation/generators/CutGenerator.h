//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATOR_H
#define OPTIMIZE_CUTGENERATOR_H

#include "AbstractCutGenerator.h"
#include "algorithms/decomposition/Generators_Basic.h"
#include "algorithms/cut-generation/original-space-builder/AbstractCutGenerationOriginalSpaceBuilder.h"

class Model;
class Algorithm;

class CutGenerator : public AbstractCutGenerator, public Generators::Basic<Var> {
protected:
    std::unique_ptr<AbstractCutGenerationOriginalSpaceBuilder> m_original_space_builder;

    static void remove_columns_violating_lower_bound(const Var& t_cut, double t_cut_primal_solution, CutGenerationSubproblem& t_subproblem);

    static void remove_columns_violating_upper_bound(const Var& t_cut, double t_cut_primal_solution, CutGenerationSubproblem& t_subproblem);
public:
    CutGenerator(const Model& t_rmp_model, const Model& t_sp_model);

    Row get_separation_objective(const Solution::Primal &t_primals) override;

    TempCtr create_cut(const Solution::Primal &t_primals) const override;

    bool set_lower_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_ub, CutGenerationSubproblem &t_subproblem) override;

    Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;

    template<class T, class ...Args> T& set_original_space_builder(Args&& ...t_args);
};

template<class T, class... Args>
T &CutGenerator::set_original_space_builder(Args&&... t_args) {
    auto* ptr = new T(std::forward<Args>(t_args)...);
    m_original_space_builder.reset(ptr);
    return *ptr;
}


#endif //OPTIMIZE_CUTGENERATOR_H
