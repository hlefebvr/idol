//
// Created by henri on 01.02.24.
//

#ifdef IDOL_USE_MIBS

#include "idol/optimizers/bilevel-optimization/wrappers/MibS/impl_MibS.h"
#include "idol/modeling/objects/Versions.h"

#include <utility>
#include <OsiSymSolverInterface.hpp>
#include <AlpsKnowledgeBrokerSerial.h>
#include <OsiCpxSolverInterface.hpp>
#include <MibSSolution.hpp>

namespace idol {
    template<class T>
    std::ostream &operator<<(std::ostream &t_os, const std::vector<T> &t_vec) {
        for (const auto &x: t_vec) {
            t_os << x << ", ";
        }
        return t_os;
    }
}

idol::impl::MibS::MibS(const idol::Model &t_model,
                       const idol::Bilevel::Description &t_description,
                       bool t_logs)
                       : m_model(t_model),
                         m_description(t_description),
                         m_logs(t_logs) {

    load_auxiliary_data();
    load_problem_data();

}
void idol::impl::MibS::load_auxiliary_data() {

    auto [upper_level_variables_indices, lower_level_variables_indices] = dispatch_variable_indices();
    auto [upper_level_constraints_indices, lower_level_constraints_indices] = dispatch_constraint_indices();
    auto lower_level_objective_coefficients = find_lower_level_objective_coefficients(lower_level_variables_indices);

    m_mibs.loadAuxiliaryData(
            (int) lower_level_variables_indices.size(),
            (int) lower_level_constraints_indices.size(),
            lower_level_variables_indices.data(),
            lower_level_constraints_indices.data(),
            1.,
            lower_level_objective_coefficients.data(),
            (int) upper_level_variables_indices.size(),
            (int) upper_level_constraints_indices.size(),
            upper_level_variables_indices.data(),
            upper_level_constraints_indices.data(),
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr
    );

}

void idol::impl::MibS::load_problem_data() {

    auto [variable_lower_bounds, variable_upper_bounds, variable_types] = parse_variables();
    auto [matrix, constraint_lower_bounds, constraint_upper_bounds, constraint_types] = parse_constraints();
    auto objective = parse_objective();

    m_mibs.loadProblemData(
            matrix,
            variable_lower_bounds.data(),
            variable_upper_bounds.data(),
            objective.data(),
            constraint_lower_bounds.data(),
            constraint_upper_bounds.data(),
            variable_types.data(),
            1.,
            Inf,
            constraint_types.data()
    );

}

void idol::impl::MibS::solve() {

    if (m_osi_solver) {
        throw Exception("Internal error: MibS::solve was called twice.");
    }

    m_osi_solver = std::make_unique<OsiSymSolverInterface>();

    m_mibs.setSolver(m_osi_solver.get());

    std::string arg = "mibs";
    int argc = 1;
    char** argv = new char* [1];
    argv[0] = arg.data();

    try {
        m_broker = std::make_unique<AlpsKnowledgeBrokerSerial>(argc, argv, m_mibs);

        if (!m_logs) {
            m_osi_solver->messageHandler()->setLogLevel(0);
            m_mibs.blisMessageHandler()->setLogLevel(0);
            m_mibs.bcpsMessageHandler()->setLogLevel(0);
            m_broker->messageHandler()->setLogLevel(0);
        }

        m_broker->search(&m_mibs);
    } catch (const CoinError& t_error) {
        throw Exception("MibS thrown an exception: " + t_error.message() + ".");
    }

}

std::pair<std::vector<int>, std::vector<int>> idol::impl::MibS::dispatch_variable_indices() {

    std::vector<int> upper_level;
    std::vector<int> lower_level;

    const auto& follower_variables = m_description.follower_vars();

    for (const auto& var : m_model.vars()) {

        auto& level = (var.get(follower_variables) == MasterId) ? upper_level : lower_level;
        const auto index = m_model.get_var_index(var);

        level.emplace_back(index);

    }

    return {
        std::move(upper_level),
        std::move(lower_level)
    };
}

std::pair<std::vector<int>, std::vector<int>> idol::impl::MibS::dispatch_constraint_indices() {

    std::vector<int> upper_level;
    std::vector<int> lower_level;

    const auto follower_obj_id = m_description.follower_obj().id();
    const auto& follower_constraints = m_description.follower_ctrs();

    unsigned int index = 0;
    for (const auto& ctr : m_model.ctrs()) {

        if (ctr.id() == follower_obj_id) {
            continue;
        }

        auto& level = (ctr.get(follower_constraints) == MasterId) ? upper_level : lower_level;

        level.emplace_back(index);
        ++index;

    }

    return {
            std::move(upper_level),
            std::move(lower_level)
    };
}

std::vector<double>
idol::impl::MibS::find_lower_level_objective_coefficients(const std::vector<int> &t_lower_level_variables_indices) {

    std::vector<double> result;
    result.reserve(t_lower_level_variables_indices.size());

    const auto& row = m_model.get_ctr_row(m_description.follower_obj());

    if (!row.quadratic().empty()) {
        throw Exception("Quadratic lower level objective is not allowed.");
    }

    for (auto index : t_lower_level_variables_indices) {
        const auto& var = m_model.get_var_by_index(index);
        const double coefficient = row.linear().get(var).as_numerical();
        result.emplace_back(coefficient);
    }

    return result;

}

std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> idol::impl::MibS::parse_variables() {

    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;
    std::vector<char> types;

    const auto n_variables = m_model.vars().size();

    lower_bounds.reserve(n_variables);
    upper_bounds.reserve(n_variables);
    types.reserve(n_variables);

    for (const auto& var : m_model.vars()) {

        const double lb = m_model.get_var_lb(var);
        const double ub = m_model.get_var_ub(var);
        const auto type = m_model.get_var_type(var);

        lower_bounds.emplace_back(lb);
        upper_bounds.emplace_back(ub);
        types.emplace_back(to_mibs_type(type));

    }

    return {
        std::move(lower_bounds),
        std::move(upper_bounds),
        std::move(types),
    };

}

std::tuple<CoinPackedMatrix, std::vector<double>, std::vector<double>, std::vector<char>>
idol::impl::MibS::parse_constraints() {

    const auto n_variables = m_model.vars().size();
    const auto n_constraints = m_model.ctrs().size() - 1; // here, we remove 1 to account for the lower level objective function

    const auto follower_obj_id = m_description.follower_obj().id();

    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;
    std::vector<char> types;

    lower_bounds.reserve(n_constraints);
    upper_bounds.reserve(n_constraints);
    types.reserve(n_constraints);

    CoinPackedMatrix matrix(false, 0, n_variables);

    for (const auto& ctr : m_model.ctrs()) {

        if (ctr.id() == follower_obj_id) {
            continue;
        }

        const auto& row = m_model.get_ctr_row(ctr);
        const auto type = m_model.get_ctr_type(ctr);
        const auto rhs = row.rhs().as_numerical();

        if (!row.quadratic().empty()) {
            throw Exception("Only linear constraints are allowed in MibS.");
        }

        switch (type) {
            case LessOrEqual:
                lower_bounds.emplace_back(-Inf);
                upper_bounds.emplace_back(rhs);
                types.emplace_back('L');
                break;
            case GreaterOrEqual:
                lower_bounds.emplace_back(rhs);
                upper_bounds.emplace_back(Inf);
                types.emplace_back('G');
                break;
            case Equal:
                lower_bounds.emplace_back(rhs);
                upper_bounds.emplace_back(rhs);
                types.emplace_back('E');
                break;
            default:
                throw Exception("Enum out of bounds.");
        }

        auto packed_vector = to_packed_vector(row.linear());
        matrix.appendRow(packed_vector);

    }

    return {
        std::move(matrix),
        std::move(lower_bounds),
        std::move(upper_bounds),
        std::move(types),
    };

}

std::vector<double> idol::impl::MibS::parse_objective() {

    const auto n_variables = m_model.vars().size();

    std::vector<double> result;
    result.reserve(n_variables);

    for (const auto& var : m_model.vars()) {

        const double coefficient = m_model.get_var_column(var).obj().as_numerical();
        result.emplace_back(coefficient);

    }

    return result;
}

char idol::impl::MibS::to_mibs_type(idol::VarType t_type) {
    switch (t_type) {
        case Continuous: return 'C';
        case Integer: return 'I';
        case Binary: return 'B';
        default:;
    }
    throw Exception("Enum out of bounds.");
}

CoinPackedVector idol::impl::MibS::to_packed_vector(const idol::LinExpr<idol::Var> &t_lin_expr) {

    CoinPackedVector result;
    result.reserve((int) t_lin_expr.size());

    for (const auto& [var, constant] : t_lin_expr) {
        const auto index = m_model.get_var_index(var);
        const double coefficient = constant.as_numerical();
        result.insert((int) index, coefficient);
    }

    return result;
}

double idol::impl::MibS::get_var_primal(const idol::Var &t_var) const {
    const unsigned int index = m_model.get_var_index(t_var);
    const auto& solution = dynamic_cast<MibSSolution&>(*m_broker->getBestKnowledge(AlpsKnowledgeTypeSolution).first);
    return solution.getValues()[index];
}

double idol::impl::MibS::get_best_obj() const {
    return m_broker->getBestQuality();
}

idol::SolutionStatus idol::impl::MibS::get_status() const {

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

idol::SolutionReason idol::impl::MibS::get_reason() const {

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

double idol::impl::MibS::get_best_bound() const {
    return m_broker->getBestEstimateQuality();
}


#endif
