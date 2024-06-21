//
// Created by henri on 20.06.24.
//

#ifdef IDOL_USE_MIBS

#include <OsiSymSolverInterface.hpp>
#include <AlpsKnowledgeBrokerSerial.h>
#include <MibSSolution.hpp>
#include <OsiCpxSolverInterface.hpp>
#include <fcntl.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/impl_MibSFromFile.h"
#include "idol/modeling/bilevel-optimization/write_to_file.h"
#include "idol/containers/Finally.h"
#include "idol/containers/SilentMode.h"

#ifdef _WIN32
#define DEV_NULL "NUL"
#else
#define DEV_NULL "/dev/null"
#endif

idol::impl::MibSFromFile::MibSFromFile(const idol::Model &t_model,
                                       const idol::Bilevel::LowerLevelDescription &t_description,
                                       bool t_logs)
                                       : m_model(t_model),
                                         m_description(t_description),
                                         m_logs(t_logs) {

}

void idol::impl::MibSFromFile::solve() {

    Bilevel::write_to_file(m_model, m_description, "bilevel");

    // WARNING: This is needed because the order of variables is changed when writing to MPS file.
    // Indeed, continuous variables are written first, then integer variables.
    make_variable_index_in_mps();

    if (m_osi_solver) {
        throw Exception("Internal error: MibS::solve was called twice.");
    }

    m_osi_solver = std::make_unique<OsiClpSolverInterface>();

    m_mibs.setSolver(m_osi_solver.get());

    const auto time_limit = std::to_string(m_model.optimizer().get_remaining_time());

    const char* argv[] = {"./mibs",
                     "-Alps_instance",
                     "bilevel.mps",
                     "-MibS_auxiliaryInfoFile",
                     "bilevel.aux",
                     "-Alps_timeLimit",
                     time_limit.data()
    };
    const int argc = 7;

    idol::SilentMode silent_mode(!m_logs);

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
    const unsigned int index = m_variable_index_in_mps[m_model.get_var_index(t_var)];
    const auto& solution = dynamic_cast<MibSSolution&>(*m_broker->getBestKnowledge(AlpsKnowledgeTypeSolution).first);
    return solution.getValues()[index];
}

double idol::impl::MibSFromFile::get_best_obj() const {
    return m_model.get_obj_expr().constant().as_numerical() + m_broker->getBestQuality();
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

void idol::impl::MibSFromFile::make_variable_index_in_mps() {

    std::list<Var> integer_variables;

    m_variable_index_in_mps.resize(m_model.vars().size());

    unsigned int index = 0;
    for (const auto& var : m_model.vars()) {

        if (m_model.get_var_type(var) == Continuous) {
            m_variable_index_in_mps[m_model.get_var_index(var)] = index;
            ++index;
        } else {
            integer_variables.push_back(var);
        }

    }

    for (const auto& var : integer_variables) {
        m_variable_index_in_mps[m_model.get_var_index(var)] = index;
        ++index;
    }

}

#endif