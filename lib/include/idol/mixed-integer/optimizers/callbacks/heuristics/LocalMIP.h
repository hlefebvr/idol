//
// Created by Henri on 27/03/2026.
//

#ifndef IDOL_LOCALMIP_H
#define IDOL_LOCALMIP_H

#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

namespace idol::Heuristics {
    template<class> class LocalMIP;
    namespace impl {
        std::optional<PrimalPoint> call_local_mip(const Model& t_model, const PrimalPoint& t_primal_point, double t_time_limit);
    }
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::Heuristics::LocalMIP : public BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy : public BranchAndBoundCallback<NodeInfoT> {
        bool m_has_been_called = false;
    protected:
        void operator()(CallbackEvent t_event) override;
    };

    BranchAndBoundCallback<NodeInfoT>* operator()() override;
    BranchAndBoundCallbackFactory<NodeInfoT>* clone() const override;
};

template <class NodeInfoT>
void idol::Heuristics::LocalMIP<NodeInfoT>::Strategy::operator()(CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    if (m_has_been_called) {
        return;
    }

    m_has_been_called = true;

    const double time_used_to_solve_node = this->relaxation().optimizer().time().count();
    const double heuristic_time_limit = std::min(this->original_model().optimizer().get_remaining_time(), 10 * time_used_to_solve_node);
    auto point = impl::call_local_mip(this->original_model(), this->node().info().primal_solution(), heuristic_time_limit);

    if (!point) {
        return;
    }

    auto* info = new NodeInfoT();
    info->set_primal_solution(*point);
    this->submit_heuristic_solution(info);

}

template <class NodeInfoT>
idol::BranchAndBoundCallback<NodeInfoT>* idol::Heuristics::LocalMIP<NodeInfoT>::operator()() {
    return new Strategy();
}

template <class NodeInfoT>
idol::BranchAndBoundCallbackFactory<NodeInfoT>* idol::Heuristics::LocalMIP<NodeInfoT>::clone() const {
    return new LocalMIP(*this);
}

#endif //IDOL_LOCALMIP_H
