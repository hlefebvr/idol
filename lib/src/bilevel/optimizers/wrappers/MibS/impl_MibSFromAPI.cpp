//
// Created by henri on 01.02.24.
//

#ifdef IDOL_USE_MIBS

#include "idol/bilevel/optimizers/wrappers/MibS/impl_MibSFromAPI.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/general/utils/SilentMode.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibSCallbackI.h"

#include <utility>

#ifdef IDOL_USE_CPLEX
#include <OsiCpxSolverInterface.hpp>
#endif

#ifdef IDOL_USE_SYMPHONY
#include <OsiSymSolverInterface.hpp>
#endif

#include <AlpsKnowledgeBrokerSerial.h>
#include <MibSSolution.hpp>

idol::impl::MibSFromAPI::MibSFromAPI(const idol::Model &t_model,
                                     const idol::Bilevel::Description &t_description,
                                     OsiSolverInterface* t_osi_solver,
                                     const std::list<std::unique_ptr<Callback>>& t_callbacks,
                                     bool t_use_cplex_for_feasibility,
                                     bool t_logs)
                       : m_model(t_model),
                         m_description(t_description),
                         m_osi_solver(t_osi_solver),
                         m_callbacks(t_callbacks),
                         m_use_cplex_for_feasibility(t_use_cplex_for_feasibility),
                         m_logs(t_logs) {

    load_auxiliary_data();
    load_problem_data();

}

void idol::impl::MibSFromAPI::load_auxiliary_data() {

    auto [upper_level_variables_indices, lower_level_variables_indices] = dispatch_variable_indices();
    auto [upper_level_constraints_indices, lower_level_constraints_indices] = dispatch_constraint_indices();
    auto lower_level_objective_coefficients = find_lower_level_objective_coefficients(lower_level_variables_indices);
    auto [lower_level_lower_bounds, lower_level_upper_bounds] = find_lower_level_bounds(lower_level_variables_indices);

    assert(upper_level_constraints_indices.size() + lower_level_constraints_indices.size() == m_n_ctr_in_mibs);

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
            lower_level_lower_bounds.data(),
            lower_level_upper_bounds.data()
    );

}

void idol::impl::MibSFromAPI::load_problem_data() {

    auto [variable_lower_bounds, variable_upper_bounds, variable_types] = parse_variables();
    auto [constraint_lower_bounds, constraint_upper_bounds, constraint_types] = parse_constraints();
    auto matrix = parse_matrix();
    auto objective = parse_objective();

    assert(constraint_lower_bounds.size() == m_n_ctr_in_mibs);
    assert(constraint_upper_bounds.size() == m_n_ctr_in_mibs);
    assert(constraint_types.size() == m_n_ctr_in_mibs);
    assert(matrix.getNumRows() == m_n_ctr_in_mibs);
    assert(variable_lower_bounds.size() == m_model.vars().size());
    assert(variable_upper_bounds.size() == m_model.vars().size());
    assert(variable_types.size() == m_model.vars().size());

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

void idol::impl::MibSFromAPI::solve() {

    m_osi_solver->messageHandler()->setLogLevel(0);
    m_mibs.setSolver(m_osi_solver.get());
    const auto time_limit = std::to_string(std::ceil(m_model.optimizer().get_remaining_time()));

    int argc = 5;
    const char* argv[] = {"./mibs",
                     "-Alps_timeLimit",
                     time_limit.data(),
                     "-feasCheckSolver",
                     m_use_cplex_for_feasibility ? "CPLEX" : "SYMPHONY"
    };

    std::unique_ptr<MibSCallbackI> callback_manager;
    if (!m_callbacks.empty()) {
        callback_manager = std::make_unique<MibSCallbackI>(*this);
        m_mibs.addHeuristic(&callback_manager->heuristic());
        m_mibs.addCutGenerator(&callback_manager->cut_generator());
    }

    idol::SilentMode silent_mode(!m_logs);

    try {
        m_broker = std::make_unique<AlpsKnowledgeBrokerSerial>(argc, (char**) argv, m_mibs, true);
        if (m_mibs.shouldInvokeSolver()) {
            m_broker->search(&m_mibs);
        }
    } catch (const CoinError& t_error) {
        std::cerr << t_error.fileName() << ":" << t_error.lineNumber() << " " << t_error.className() << "::" << t_error.methodName() << " " << t_error.message() << std::endl;
        throw Exception("MibS thrown an exception: " + t_error.message() + ".");
    }

}

std::pair<std::vector<int>, std::vector<int>> idol::impl::MibSFromAPI::dispatch_variable_indices() {

    std::vector<int> upper_level;
    std::vector<int> lower_level;

    for (const auto& var : m_model.vars()) {

        if (m_description.is_lower(var)) {
            const auto index = m_model.get_var_index(var);
            lower_level.emplace_back(index);
        }

    }

    for (const auto& var : m_model.vars()) {

        if (m_description.is_upper(var) && m_model.get_var_type(var) != Continuous) {
            const auto index = m_model.get_var_index(var);
            upper_level.emplace_back(index);
        }

    }

    for (const auto& var : m_model.vars()) {

        if (m_description.is_upper(var) && m_model.get_var_type(var) == Continuous) {
            const auto index = m_model.get_var_index(var);
            upper_level.emplace_back(index);
        }

    }

    assert(upper_level.size() + lower_level.size() == m_model.vars().size());

    return {
        std::move(upper_level),
        std::move(lower_level)
    };
}

std::pair<std::vector<int>, std::vector<int>> idol::impl::MibSFromAPI::dispatch_constraint_indices() {

    std::vector<int> upper_level;
    std::vector<int> lower_level;

    const unsigned int n_ctrs = m_model.ctrs().size();
    m_ctr_indices_in_mibs.resize(n_ctrs);

    unsigned int index_in_mibs = 0;
    for (unsigned int i = 0 ; i < n_ctrs ; ++i) {

        const auto& ctr = m_model.get_ctr_by_index(i);
        auto& level = m_description.is_upper(ctr) ? upper_level : lower_level;
        auto type = m_model.get_ctr_type(ctr);

        level.emplace_back(index_in_mibs);
        ++index_in_mibs;

        if (type == Equal) {
            m_ctr_indices_in_mibs[i] = { index_in_mibs - 1, index_in_mibs };
            level.emplace_back(index_in_mibs);
            ++index_in_mibs;
        } else {
            m_ctr_indices_in_mibs[i] = { index_in_mibs - 1 };
        }

        m_n_ctr_in_mibs += m_ctr_indices_in_mibs[i].size();

    }

    return {
            std::move(upper_level),
            std::move(lower_level)
    };
}

std::vector<double>
idol::impl::MibSFromAPI::find_lower_level_objective_coefficients(const std::vector<int> &t_lower_level_variables_indices) {

    std::vector<double> result;
    result.reserve(t_lower_level_variables_indices.size());

    const auto& follower_obj = m_description.lower_level_obj();

    for (const auto& var_id : t_lower_level_variables_indices) {

        const auto& var = m_model.get_var_by_index(var_id);
        const double coefficient = follower_obj.affine().linear().get(var);

        result.emplace_back(coefficient);

    }

    return result;

}

std::pair<std::vector<double>, std::vector<double>>
idol::impl::MibSFromAPI::find_lower_level_bounds(const std::vector<int> &t_lower_level_variables_indices) {

    std::vector<double> lb, ub;
    lb.reserve(t_lower_level_variables_indices.size());
    ub.reserve(t_lower_level_variables_indices.size());

    for (const auto& var_id : t_lower_level_variables_indices) {

        const auto& var = m_model.get_var_by_index(var_id);
        const double lower_bound = m_model.get_var_lb(var);
        const double upper_bound = m_model.get_var_ub(var);

        lb.emplace_back(is_neg_inf(lower_bound) ? -m_osi_solver->getInfinity() : lower_bound);
        ub.emplace_back(is_pos_inf(upper_bound) ? m_osi_solver->getInfinity() : upper_bound);

    }

    assert(t_lower_level_variables_indices.size() == lb.size() && t_lower_level_variables_indices.size() == ub.size());

    return {
        std::move(lb),
        std::move(ub)
    };

}

std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> idol::impl::MibSFromAPI::parse_variables() {

    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;
    std::vector<char> types;

    const auto n_variables = m_model.vars().size();

    lower_bounds.reserve(n_variables);
    upper_bounds.reserve(n_variables);
    types.reserve(n_variables);

    for (const auto& var : m_model.vars()) {

        const auto type = m_model.get_var_type(var);
        const double lb = m_model.get_var_lb(var);
        const double ub = m_model.get_var_ub(var);

        types.emplace_back(to_mibs_type(type));
        lower_bounds.emplace_back(lb);
        upper_bounds.emplace_back(ub);

    }

    return {
        std::move(lower_bounds),
        std::move(upper_bounds),
        std::move(types),
    };

}

std::tuple<std::vector<double>, std::vector<double>, std::vector<char>>
idol::impl::MibSFromAPI::parse_constraints() {

    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;
    std::vector<char> types;

    lower_bounds.reserve(m_n_ctr_in_mibs);
    upper_bounds.reserve(m_n_ctr_in_mibs);
    types.reserve(m_n_ctr_in_mibs);

    for (unsigned int i = 0, n = m_model.ctrs().size() ; i < n ; ++i) {

        const auto ctr = m_model.get_ctr_by_index(i);
        const auto type = m_model.get_ctr_type(ctr);
        const auto rhs = m_model.get_ctr_rhs(ctr);

        assert(!is_inf(rhs));

        switch (type) {
            case LessOrEqual:
                lower_bounds.emplace_back(-std::numeric_limits<double>::infinity());
                upper_bounds.emplace_back(rhs);
                types.emplace_back('L');
                break;
            case GreaterOrEqual:
                lower_bounds.emplace_back(rhs);
                upper_bounds.emplace_back(std::numeric_limits<double>::infinity());
                types.emplace_back('G');
                break;
            case Equal:
                lower_bounds.emplace_back(rhs);
                upper_bounds.emplace_back(std::numeric_limits<double>::infinity());
                types.emplace_back('G');

                lower_bounds.emplace_back(-std::numeric_limits<double>::infinity());
                upper_bounds.emplace_back(rhs);
                types.emplace_back('L');
                break;
            default:
                throw Exception("Enum out of bounds.");
        }

    }

    return {
        std::move(lower_bounds),
        std::move(upper_bounds),
        std::move(types),
    };

}

CoinPackedMatrix idol::impl::MibSFromAPI::parse_matrix() {

    const unsigned int n_variables = m_model.vars().size();

    CoinPackedMatrix result(true, n_variables, 0);

    for (const auto& var : m_model.vars()) {

        const auto& col = m_model.get_var_column(var);

        const auto& lin = col;

        CoinPackedVector vector;
        vector.reserve((int) lin.size());
        for (const auto& [ctr, constant] : lin) {
            for (auto index : m_ctr_indices_in_mibs[m_model.get_ctr_index(ctr)]) {
                vector.insert((int) index, constant);
            }
        }

        vector.sortIncrIndex();

        result.appendCol(vector);
    }

    return result;
}


std::vector<double> idol::impl::MibSFromAPI::parse_objective() {

    const auto n_variables = m_model.vars().size();

    std::vector<double> result;
    result.reserve(n_variables);

    for (const auto& var : m_model.vars()) {

        const double coefficient = m_model.get_var_obj(var);
        result.emplace_back(coefficient);

    }

    return result;
}

char idol::impl::MibSFromAPI::to_mibs_type(idol::VarType t_type) {
    switch (t_type) {
        case Continuous: return 'C';
        case Integer: return 'I';
        case Binary: return 'B';
        default:;
    }
    throw Exception("Enum out of bounds.");
}

double idol::impl::MibSFromAPI::get_var_primal(const idol::Var &t_var) const {
    const unsigned int index = m_model.get_var_index(t_var);
    const auto& solution = dynamic_cast<MibSSolution&>(*m_broker->getBestKnowledge(AlpsKnowledgeTypeSolution).first);
    return solution.getValues()[index];
}

double idol::impl::MibSFromAPI::get_best_obj() const {
    return m_broker->getBestQuality();
}

idol::SolutionStatus idol::impl::MibSFromAPI::get_status() const {

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

idol::SolutionReason idol::impl::MibSFromAPI::get_reason() const {

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

double idol::impl::MibSFromAPI::get_best_bound() const {
    if (get_status() == Optimal) {
        return get_best_obj();
    }
    const auto *node = m_broker->getBestNode();
    if (node) {
        return node->getQuality();
    }
    return -Inf;
}

#endif
