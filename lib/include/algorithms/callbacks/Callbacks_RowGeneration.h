//
// Created by henri on 13/10/22.
//

#ifndef IDOL_CALLBACKS_ROWGENERATION_H
#define IDOL_CALLBACKS_ROWGENERATION_H

#include "../row-generation/RowGeneration.h"
#include "../callbacks/Callback.h"
#include "../callbacks/AlgorithmInCallback.h"

class AlgorithmInCallback;

namespace Callbacks {
    class RowGeneration;
}

class Callbacks::RowGeneration : public Callback, private ::RowGeneration {
    AlgorithmInCallback& m_proxy;
    std::vector<Event> m_events;
protected:
    void solve_rmp() override {}
public:
    template<class ...ArgsT> explicit RowGeneration(Algorithm& t_algorithm, Event t_head = NewIncumbentFound, ArgsT&& ...t_triggers);

    ~RowGeneration() override;

    void execute(Context& t_ctx) override;

    RowGenerationSP& add_subproblem(const Ctr& t_ctr);

    [[nodiscard]] Solution::Primal help() const override;

    void update_var_lb(const Var &t_var, double t_lb) override;

    void update_var_ub(const Var &t_var, double t_lb) override;

    using ::RowGeneration::subproblems;

    static const bool uses_lazy_cuts = true;
    static const bool uses_advanced_constructor = true;
};

template<class... ArgsT>
Callbacks::RowGeneration::RowGeneration(Algorithm &t_algorithm, Event t_head, ArgsT &&... t_triggers)
        : ::RowGeneration(*new AlgorithmInCallback(t_algorithm)),
          m_proxy(dynamic_cast<AlgorithmInCallback&>(rmp_solution_strategy())),
          m_events({ t_head, t_triggers... }) {

    RowGeneration::set(Param::Algorithm::MaxIterations, 1);

}

#endif //IDOL_CALLBACKS_ROWGENERATION_H
