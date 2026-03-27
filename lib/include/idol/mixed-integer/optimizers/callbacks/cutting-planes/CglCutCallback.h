//
// Created by Henri on 25/03/2026.
//

#ifndef IDOL_CGLCUTS_H
#define IDOL_CGLCUTS_H

#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CutFamily.h"

#ifdef IDOL_USE_CGL
#include <CglKnapsackCover.hpp>
#include <CglFlowCover.hpp>
#include <CglZeroHalf.hpp>
#include <CglMixedIntegerRounding.hpp>
#include <CglOddHole.hpp>
#include <CglLandP.hpp>
#include <CglResidualCapacity.hpp>
#else
class OsiRowCut;
#endif

namespace idol {
    template<class> class CglCutCallback;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::CglCutCallback : public BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy : public BranchAndBoundCallback<NodeInfoT> {

        class NodeCutContext {
            const unsigned int m_node_id;
            unsigned int m_pass_count = 0;
            unsigned int m_n_added_cuts = 0;
        public:
            NodeCutContext(unsigned int node_id) : m_node_id(node_id) {}

            [[nodiscard]] unsigned int node_id() const { return m_node_id; }
            [[nodiscard]] bool is_root_node() const { return (m_node_id == 0); }
            [[nodiscard]] unsigned int pass() const { return m_pass_count; }
            [[nodiscard]] unsigned int n_added_cuts() const { return m_n_added_cuts; }

            void increment_pass() { m_pass_count++; }
            void add_accepted_cut() { m_n_added_cuts++; }
        };

        class OsiIdolCglSolverInterface;
#ifdef IDOL_USE_CGL
        std::unique_ptr<OsiIdolCglSolverInterface> m_osi_solver;
#endif
        std::vector<std::unique_ptr<CutFamily>> m_cut_families;
        std::unique_ptr<NodeCutContext> m_cut_context;

        unsigned int m_total_n_added_cuts = 0;
        const unsigned int m_max_pass_at_root_node = 5;
        const unsigned int m_max_pass_per_node_in_tree = 1;
        const unsigned int m_max_depth_for_cuts = 5;
        const double m_effectiveness_threshold = 1e-3;
        const unsigned int m_max_cut_at_root_node = std::numeric_limits<unsigned int>::max();
        const unsigned int m_max_cut_per_node = std::numeric_limits<unsigned int>::max();
        const unsigned int m_max_cuts = std::numeric_limits<unsigned int>::max();

        std::list<TempCtr> to_idol_cuts(OsiCuts& t_cuts);
        TempCtr to_idol_cut(OsiRowCut& t_cut);
        std::vector<std::pair<TempCtr, double>> sort_cuts_by_effectiveness(const std::list<TempCtr>& t_cuts);
    protected:
        NodeCutContext& get_cut_context();
        const NodeCutContext& get_cut_context() const { return const_cast<Strategy*>(this)->get_cut_context(); }
        void initialize() override;
        void log_after_termination() override;
        void operator()(CallbackEvent t_event) override;
    };

    Strategy* operator()() override;
    [[nodiscard]] CglCutCallback* clone() const override;
};

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>::Strategy* idol::CglCutCallback<NodeInfoT>::operator()() {
    return new Strategy();
}

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>* idol::CglCutCallback<NodeInfoT>::clone() const {
    return new CglCutCallback<NodeInfoT>(*this);
}

#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/OsiIdolCglSolverInterface.h"

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::initialize() {
#ifdef IDOL_USE_CGL

    BranchAndBoundCallback<NodeInfoT>::initialize();

    m_osi_solver = std::make_unique<OsiIdolCglSolverInterface>(*this);

    m_cut_families.emplace_back(new CutFamily(new CglKnapsackCover(), "Cover"));
    m_cut_families.emplace_back(new CutFamily(new CglFlowCover(), "Flow Cover"));
    m_cut_families.emplace_back(new CutFamily(new CglZeroHalf(), "Zero Half"));
    m_cut_families.emplace_back(new CutFamily(new CglMixedIntegerRounding(), "MIR"));
    m_cut_families.emplace_back(new CutFamily(new CglResidualCapacity(), "Residual Capacity"));
    //m_cut_families.emplace_back(new CutFamily(new CglLandP(), "Lift-and-Project")); // Needs getObjValue
    //m_cut_families.emplace_back(new CutFamily(new CglOddHole(), "Odd Hole")); // Needs reduced costs

#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::log_after_termination() {

    BranchAndBoundCallback<NodeInfoT>::log_after_termination();

    for (const auto& cut_family : m_cut_families) {
        std::cout << '\t' << cut_family->name() << " : " << cut_family->n_accepted()
                  << " (score: " << cut_family->score() << ","
                  << " effectiveness: " << cut_family->average_effectiveness() << ","
                  << " time per call: " << cut_family->average_time_per_call()
                  << ")\n";
    }

}

template <class NodeInfoT>
void idol::CglCutCallback<NodeInfoT>::Strategy::operator()(CallbackEvent t_event) {
#ifdef IDOL_USE_CGL
    if (t_event != InvalidSolution) {
        return;
    }

    if (m_total_n_added_cuts > m_max_cuts) {
        return;
    }

    if (this->node().level() > m_max_depth_for_cuts) {
        return;
    }

    if (relative_gap(this->best_bound(), this->best_obj()) < .1) {
        return;
    }

    auto& cut_context = get_cut_context();
    const bool is_root_node = cut_context.is_root_node();

    unsigned int max_n_added_cut_at_this_node = m_max_cut_per_node;
    unsigned int max_pass_at_this_node = m_max_pass_per_node_in_tree;
    if (is_root_node) {
        max_n_added_cut_at_this_node = m_max_cut_at_root_node;
        max_pass_at_this_node = m_max_pass_at_root_node;
    }

    if (cut_context.pass() > max_pass_at_this_node) {
        return;
    }

    cut_context.increment_pass();

    // Count number of fractional over integer ?

    if (!is_root_node) {
        std::sort(m_cut_families.begin(), m_cut_families.end(), [](const auto& t_a, const auto& t_b) {
            return t_a->score() > t_b->score();
        });
    }

    m_osi_solver->update_current_solution();

    for (auto& cut_family : m_cut_families) {

        if (!is_root_node && cut_family->score() <= 1e-2) {
            continue;
        }

        auto osi_cuts = cut_family->generate(*m_osi_solver, is_root_node ? 100 : 0);
        auto idol_cuts = to_idol_cuts(osi_cuts);
        auto sorted_cuts = sort_cuts_by_effectiveness(idol_cuts);

        for (auto& [cut, effectiveness] : sorted_cuts) {

            if (m_total_n_added_cuts > m_max_cuts) {
                return;
            }

            if (cut_context.n_added_cuts() > max_n_added_cut_at_this_node) {
                return;
            }

            cut_family->add_effectiveness_statistics(effectiveness);

            if (effectiveness <= m_effectiveness_threshold) {
                continue;
            }

            this->add_user_cut(cut);
            m_total_n_added_cuts++;
            cut_context.add_accepted_cut();
            cut_family->add_acceptance_statistics(1);
        }

    }
#endif
}

template <class NodeInfoT>
std::list<idol::TempCtr> idol::CglCutCallback<NodeInfoT>::Strategy::to_idol_cuts(OsiCuts& t_cuts) {
#ifdef IDOL_USE_CGL
    std::list<TempCtr> result;

    for (unsigned int k = 0, n = t_cuts.sizeRowCuts() ; k < n ; k++) {
        auto& cut = *t_cuts.rowCutPtr(k);

        if (!cut.consistent()) {
            continue;
        }

        if (cut.infeasible(*m_osi_solver)) {
            continue;
        }

        result.emplace_back(to_idol_cut(cut));
    }

    return result;
#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
idol::TempCtr idol::CglCutCallback<NodeInfoT>::Strategy::to_idol_cut(OsiRowCut& t_cut) {
#ifdef IDOL_USE_CGL
    const auto& model = this->original_model();

    TempCtr cut;

    const auto osi_sense = t_cut.sense();
    if (osi_sense == 'G') {
        cut.set_type(GreaterOrEqual);
        cut.set_rhs(t_cut.lb());
    } else if (osi_sense == 'L') {
        cut.set_type(LessOrEqual);
        cut.set_rhs(t_cut.ub());
    } else if (osi_sense == 'R') {
        if (const double lb = t_cut.lb() ; !is_neg_inf(lb)) {
            cut.set_type(GreaterOrEqual);
            cut.set_rhs(t_cut.lb());
        } else if (const double ub = t_cut.ub() ; !is_pos_inf(ub)) {
            cut.set_type(LessOrEqual);
            cut.set_rhs(t_cut.ub());
        } else {
            throw Exception("Cgl returned a cut of type R, which is not handled in idol.");
        }
    }
    else {
        throw Exception("Could not handle cut type from Cgl.");
    }

    const auto& osi_row = t_cut.row();
    const auto* indices = osi_row.getIndices();
    const auto* values = osi_row.getElements();

    for (unsigned int k = 0, n = osi_row.getNumElements() ; k < n ; k++) {
        const auto& var = model.get_var_by_index(indices[k]);
        cut.lhs() += values[k] * var;
    }

    return cut;
#else
    throw Exception("idol was not linked with Cgl.");
#endif
}

template <class NodeInfoT>
std::vector<std::pair<idol::TempCtr, double>> idol::CglCutCallback<NodeInfoT>::Strategy::sort_cuts_by_effectiveness(const std::list<TempCtr>& t_cuts) {

    std::vector<std::pair<TempCtr, double>> result;
    const auto primal_solution = this->node().info().primal_solution();

    for (auto& cut : t_cuts) {

        double activity = 0.;
        double norm = 0.;

        for (const auto& [var, coefficient] : cut.lhs()) {
            norm += coefficient * coefficient;
            activity += coefficient * primal_solution.get(var);
        }
        norm = std::sqrt(norm);

        double effectiveness = (activity - cut.rhs()) / norm;
        if (cut.type() == GreaterOrEqual) {
            effectiveness *= -1.;
        }

        result.emplace_back(std::move(cut), effectiveness);

    }

    std::sort(result.begin(), result.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.second > t_b.second;
    });

    return result;
}

template <class NodeInfoT>
idol::CglCutCallback<NodeInfoT>::Strategy::NodeCutContext& idol::CglCutCallback<NodeInfoT>::Strategy::get_cut_context() {

    const auto current_node_id = this->node().id();

    if (!m_cut_context || m_cut_context->node_id() != current_node_id) {
        m_cut_context.reset(new NodeCutContext(current_node_id));
    }

    return *m_cut_context;
}

#endif //IDOL_CGLCUTS_H
