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
        bool m_found_feasible_at_root_node = true;
        unsigned int m_n_calls = 0;
        unsigned int m_n_tries = 0;
        unsigned int m_n_submitted_points = 0;
        unsigned int m_frequency = 50;
    protected:
        void operator()(CallbackEvent t_event) override;
        void log_after_termination() override;
    };

    BranchAndBoundCallback<NodeInfoT>* operator()() override;
    BranchAndBoundCallbackFactory<NodeInfoT>* clone() const override;
};

template <class NodeInfoT>
void idol::Heuristics::LocalMIP<NodeInfoT>::Strategy::operator()(CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    const bool is_root_node = this->node().id() == 0;

    m_n_calls++;
    if (!is_root_node) {
        if (m_n_calls % m_frequency > 0 || !m_found_feasible_at_root_node) {
            return;
        }
    }

    const double relative_gap = idol::relative_gap(this->best_bound(), this->best_obj());

    bool be_aggressive = false;
    if (is_root_node || relative_gap > 1) {
        be_aggressive = true;
    }

    const double time_used_to_solve_node = this->relaxation().optimizer().time().count();
    const double factor_for_heuristic = be_aggressive ? 200 : 10;
    const double heuristic_time_limit = std::min(this->original_model().optimizer().get_remaining_time(), factor_for_heuristic * time_used_to_solve_node);
    auto point = impl::call_local_mip(this->original_model(), this->node().info().primal_solution(), heuristic_time_limit);
    m_n_tries++;

    if (!point) {

        if (is_root_node) {
            m_found_feasible_at_root_node = false;
        }

        if (m_n_submitted_points == 0) {
            m_frequency *= 1.5;
        }

        return;
    }

    auto* info = new NodeInfoT();
    info->set_primal_solution(*point);
    this->submit_heuristic_solution(info);
    m_n_submitted_points++;

}

template <class NodeInfoT>
void idol::Heuristics::LocalMIP<NodeInfoT>::Strategy::log_after_termination() {

    BranchAndBoundCallback<NodeInfoT>::log_after_termination();

    std::cout << "Local-MIP Heuristic: Called " << m_n_tries << ", Found " << m_n_submitted_points << "." << std::endl;

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
