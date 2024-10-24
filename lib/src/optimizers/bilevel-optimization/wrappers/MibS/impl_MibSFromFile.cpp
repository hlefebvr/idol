//
// Created by henri on 20.06.24.
//

#ifdef IDOL_USE_MIBS

#include <OsiSymSolverInterface.hpp>
#include <AlpsKnowledgeBrokerSerial.h>
#include <MibSSolution.hpp>
#include <OsiCpxSolverInterface.hpp>
#include <fcntl.h>
#include <filesystem>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/impl_MibSFromFile.h"
#include "idol/modeling/bilevel-optimization/write_to_file.h"
#include "idol/utils/Finally.h"
#include "idol/utils/SilentMode.h"
#include "idol/utils/uuid.h"

#ifdef _WIN32
#define DEV_NULL "NUL"
#else
#define DEV_NULL "/dev/null"
#endif

idol::impl::MibSFromFile::MibSFromFile(const idol::Model &t_model,
                                       const idol::Bilevel::LowerLevelDescription &t_description,
                                       OsiSolverInterface* t_osi_solver,
                                       bool t_use_cplex_for_feasibility,
                                       bool t_logs)
                                       : m_model(t_model),
                                         m_description(t_description),
                                         m_osi_solver(t_osi_solver),
                                         m_use_cplex_for_feasibility(t_use_cplex_for_feasibility),
                                         m_logs(t_logs) {

}

void idol::impl::MibSFromFile::solve() {

    if (!m_variable_index_in_mps.empty()) {
        throw Exception("solve() has already been called.");
    }

    make_variable_index_in_mps();

    const std::string filename = idol::generate_uuid_v4() + "-bilevel";
    const std::string mps_filename = filename + ".mps";
    const std::string aux_filename = filename + ".aux";

    idol::Finally finally([&mps_filename, &aux_filename] {
        std::filesystem::remove(mps_filename.c_str());
        std::filesystem::remove(aux_filename.c_str());
    });

    Bilevel::write_to_file(m_model, m_description, filename);

    m_osi_solver->messageHandler()->setLogLevel(0);

    m_mibs.setSolver(m_osi_solver.get());

    const auto time_limit = std::to_string(m_model.optimizer().get_remaining_time());

    const int argc = 9;
    const char* argv[] = {"./mibs",
                     "-Alps_instance",
                     mps_filename.c_str(),
                     "-MibS_auxiliaryInfoFile",
                     aux_filename.c_str(),
                     "-Alps_timeLimit",
                     time_limit.data(),
                     "-feasCheckSolver",
                     m_use_cplex_for_feasibility ? "CPLEX" : "SYMPHONY"
    };

    idol::SilentMode silent_mode(!m_logs);

    try {
        m_broker = std::make_unique<AlpsKnowledgeBrokerSerial>(argc, (char**) argv, m_mibs, false);

        if (m_mibs.shouldInvokeSolver()) {
            m_broker->search(&m_mibs);
        }

    } catch (const CoinError& t_error) {
        std::cerr << t_error.fileName() << ":" << t_error.lineNumber() << " " << t_error.className() << "::" << t_error.methodName() << " " << t_error.message() << std::endl;
        throw Exception("MibS thrown an exception: " + t_error.message() + ".");
    }

}


double idol::impl::MibSFromFile::get_var_primal(const idol::Var &t_var) const {
    const unsigned int index = m_variable_index_in_mps[m_model.get_var_index(t_var)];
    const auto& solution = dynamic_cast<MibSSolution&>(*m_broker->getBestKnowledge(AlpsKnowledgeTypeSolution).first);
    return solution.getValues()[index];
}

double idol::impl::MibSFromFile::get_best_obj() const {
    return m_model.get_obj_expr().constant() + m_broker->getBestQuality();
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
    if (get_status() == Optimal) {
        return get_best_obj();
    }
    const auto *node = m_broker->getBestNode();
    if (node) {
        return node->getQuality();
    }
    return -Inf;
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