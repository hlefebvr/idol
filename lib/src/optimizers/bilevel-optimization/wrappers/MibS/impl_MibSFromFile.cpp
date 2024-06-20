//
// Created by henri on 20.06.24.
//

#ifdef IDOL_USE_MIBS

#include <OsiSymSolverInterface.hpp>
#include <AlpsKnowledgeBrokerSerial.h>
#include <MibSSolution.hpp>
#include <OsiCpxSolverInterface.hpp>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/impl_MibSFromFile.h"
#include "idol/modeling/bilevel-optimization/write_to_file.h"

idol::impl::MibSFromFile::MibSFromFile(const idol::Model &t_model,
                                       const idol::Bilevel::Description &t_description,
                                       bool t_logs)
                                       : m_model(t_model),
                                         m_description(t_description),
                                         m_logs(t_logs) {

}

void idol::impl::MibSFromFile::solve() {

    Bilevel::write_to_file(m_model, m_description, "bilevel");

    if (m_osi_solver) {
        throw Exception("Internal error: MibS::solve was called twice.");
    }

    m_osi_solver = std::make_unique<OsiClpSolverInterface>();

    m_mibs.setSolver(m_osi_solver.get());

    const char* argv[] = {"./mibs",
                     "-Alps_instance",
                     "bilevel.mps",
                     "-MibS_auxiliaryInfoFile",
                     "bilevel.aux"};
    const int argc = 5;

    try {
        m_broker = std::make_unique<AlpsKnowledgeBrokerSerial>(argc, (char**) argv, m_mibs, false);

        if (!m_logs) {
            m_osi_solver->messageHandler()->setLogLevel(0);
            m_mibs.blisMessageHandler()->setLogLevel(0);
            m_mibs.bcpsMessageHandler()->setLogLevel(0);
            m_broker->messageHandler()->setLogLevel(0);
        }

        if (m_mibs.shouldInvokeSolver()) {
            m_broker->search(&m_mibs);
        }

    } catch (const CoinError& t_error) {
        throw Exception("MibS: " + t_error.message() + ".");
    }

}


double idol::impl::MibSFromFile::get_var_primal(const idol::Var &t_var) const {
    const unsigned int index = m_model.get_var_index(t_var);
    const auto& solution = dynamic_cast<MibSSolution&>(*m_broker->getBestKnowledge(AlpsKnowledgeTypeSolution).first);
    return solution.getValues()[index];
}

double idol::impl::MibSFromFile::get_best_obj() const {
    return m_broker->getBestQuality();
}

idol::SolutionStatus idol::impl::MibSFromFile::get_status() const {

    switch (m_broker->getSolStatus()) {
        case AlpsExitStatusUnknown: return Fail;
        case AlpsExitStatusOptimal: return Optimal;
        case AlpsExitStatusFeasible: return Feasible;
        case AlpsExitStatusInfeasible: return Infeasible;
        case AlpsExitStatusFailed: return Fail;
        case AlpsExitStatusUnbounded: return Unbounded;
        case AlpsExitStatusNodeLimit: [[fallthrough]];
        case AlpsExitStatusTimeLimit: [[fallthrough]];
        case AlpsExitStatusNoMemory: [[fallthrough]];
        case AlpsExitStatusSolLimit: return m_broker->getBestNode() ? Feasible : Infeasible;
    }

    throw Exception("enum out of bounds.");

}

idol::SolutionReason idol::impl::MibSFromFile::get_reason() const {

    switch (m_broker->getSolStatus()) {
        case AlpsExitStatusOptimal:  [[fallthrough]];
        case AlpsExitStatusFeasible: [[fallthrough]];
        case AlpsExitStatusUnbounded: [[fallthrough]];
        case AlpsExitStatusInfeasible: return Proved;
        case AlpsExitStatusUnknown: [[fallthrough]];
        case AlpsExitStatusFailed: return NotSpecified;
        case AlpsExitStatusNodeLimit: return IterLimit;
        case AlpsExitStatusTimeLimit: return TimeLimit;
        case AlpsExitStatusNoMemory: return MemoryLimit;
        case AlpsExitStatusSolLimit: return ObjLimit;
    }

    throw Exception("enum out of bounds.");
}

double idol::impl::MibSFromFile::get_best_bound() const {
    return m_broker->getBestEstimateQuality();
}

#endif