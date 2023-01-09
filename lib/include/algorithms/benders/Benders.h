//
// Created by henri on 04/01/23.
//

#ifndef IDOL_BENDERS_H
#define IDOL_BENDERS_H

#include "reformulations/Reformulations_Benders.h"
#include "../Algorithm.h"
#include "BendersSP.h"
#include "Callbacks_Benders.h"

class Benders : public Algorithm {
    Reformulations::Benders m_reformulation;
    std::vector<BendersSP> m_subproblems;
    std::unique_ptr<Algorithm> m_master_solution_strategy;
protected:
    void execute() override;

    void initialize();
public:
    Benders(Model& t_model, const UserAttr& t_subproblem_flag);

    Reformulations::Benders& reformulation() { return m_reformulation; }
    [[nodiscard]] const Reformulations::Benders& reformulation() const { return m_reformulation; }

    BendersSP& subproblem(unsigned int t_index) { return m_subproblems[t_index-1]; }
    [[nodiscard]] const BendersSP& subproblem(unsigned int t_index) const { return m_subproblems[t_index-1]; }

    IteratorForward<std::vector<BendersSP>> subproblems() { return m_subproblems; }
    [[nodiscard]] ConstIteratorForward<std::vector<BendersSP>> subproblems() const { return m_subproblems; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_master_solution_strategy(ArgsT&& ...t_args);

    Algorithm& master_solution_strategy() { return *m_master_solution_strategy; }
    [[nodiscard]] const Algorithm& master_solution_strategy() const { return *m_master_solution_strategy; }

    [[nodiscard]] Solution::Primal primal_solution() const override;

    using Algorithm::set;
    using Algorithm::get;
};

template<class AlgorithmT, class... ArgsT>
AlgorithmT &Benders::set_master_solution_strategy(ArgsT &&... t_args) {
    auto* result = new AlgorithmT(m_reformulation.master_problem(), std::forward<ArgsT>(t_args)...);
    result->template set_user_callback<Callbacks::Benders>(*this);
    m_master_solution_strategy.reset(result);
    return *result;
}

#endif //IDOL_BENDERS_H
