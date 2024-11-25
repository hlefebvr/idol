//
// Created by henri on 24.11.24.
//

#ifndef IDOL_REDUCEDCOSTFIXING_H
#define IDOL_REDUCEDCOSTFIXING_H

#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"

namespace idol {
    template<class> class ReducedCostFixing;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::ReducedCostFixing : public BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy : public BranchAndBoundCallback<NodeInfoT> {
    public:
    protected:
        void operator()(CallbackEvent t_event) override {

            if (t_event != InvalidSolution) {
                return;
            }

            const auto& original_model = this->original_model();
            const auto& relaxation = this->relaxation();
            const double best_obj = this->best_obj();
            const double current_obj = this->relaxation().get_best_obj();

            for (const auto &var : original_model.vars()) {

                double reduced_cost = relaxation.get_var_reduced_cost(var);

                if (current_obj + reduced_cost > best_obj + Tolerance::MIPAbsoluteGap) {
                    const double relaxation_lb = relaxation.get_var_lb(var);
                    const double current_ub = relaxation.get_var_ub(var);
                    if (!equals(relaxation_lb, current_ub, Tolerance::Integer)) {
                        this->add_local_variable_branching(var, LessOrEqual, relaxation_lb);
                    }
                }

                if (current_obj - reduced_cost > best_obj + Tolerance::MIPAbsoluteGap) {
                    const double relaxation_ub = relaxation.get_var_ub(var);
                    const double current_lb = relaxation.get_var_lb(var);
                    if (!equals(relaxation_ub, current_lb, Tolerance::Integer)) {
                        this->add_local_variable_branching(var, GreaterOrEqual, relaxation_ub);
                    }
                }

            }

        }
    };

    BranchAndBoundCallback<NodeInfoT> *operator()() override {
        return new Strategy();
    }

    BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override {
        return new ReducedCostFixing();
    }
};

#endif //IDOL_REDUCEDCOSTFIXING_H
