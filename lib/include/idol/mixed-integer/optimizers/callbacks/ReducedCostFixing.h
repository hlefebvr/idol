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
        unsigned int m_last_node_id = -1;
    protected:
        void operator()(CallbackEvent t_event) override {

            if (t_event != InvalidSolution) {
                return;
            }

            if (m_last_node_id == this->node().id()) {
                return;
            }
            m_last_node_id = this->node().id();

            const auto& original_model = this->original_model();
            const auto& relaxation = this->relaxation();
            const double best_obj = this->best_obj();
            const double current_obj = this->relaxation().get_best_obj();
            if (is_pos_inf(best_obj)) {
                return;
            }
            const double tol_mip_absolute_gap = original_model.optimizer().get_tol_mip_absolute_gap();
            const double tol_integer = original_model.optimizer().get_tol_integer();
            const double tol_optimality = relaxation.optimizer().get_tol_optimality();
            for (const auto &var : original_model.vars()) {

                if (original_model.get_var_type(var) == Continuous) {
                    continue;
                }

                const double reduced_cost = relaxation.get_var_reduced_cost(var);
                const double primal = relaxation.get_var_primal(var);
                const double relaxation_lb = relaxation.get_var_lb(var);
                const double relaxation_ub = relaxation.get_var_ub(var);

                if (!is_neg_inf(relaxation_lb) &&
                    is_integer(relaxation_lb, tol_integer) &&
                    equals(primal, relaxation_lb, tol_integer) &&
                    reduced_cost > tol_optimality &&
                    current_obj + reduced_cost > best_obj + tol_mip_absolute_gap) {
                    this->add_local_variable_branching(var, LessOrEqual, relaxation_lb);
                }

                if (!is_pos_inf(relaxation_ub) &&
                    is_integer(relaxation_ub, tol_integer) &&
                    equals(primal, relaxation_ub, tol_integer) &&
                    reduced_cost < -tol_optimality &&
                    current_obj - reduced_cost > best_obj + tol_mip_absolute_gap) {
                    this->add_local_variable_branching(var, GreaterOrEqual, relaxation_ub);
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
