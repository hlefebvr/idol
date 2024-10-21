//
// Created by henri on 01.02.24.
//

#ifdef IDOL_USE_MIBS

#ifndef IDOL_IMPL_MIBS_FROM_API_H
#define IDOL_IMPL_MIBS_FROM_API_H

#include "idol/modeling/models/Model.h"

#include "MibSModel.hpp"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "impl_MibS.h"

namespace idol::impl {
    class MibSFromAPI;
}

class idol::impl::MibSFromAPI : public idol::impl::MibS {
    const idol::Model& m_model;
    const idol::Bilevel::LowerLevelDescription& m_description;
    const bool m_logs;

    MibSModel m_mibs;
    std::unique_ptr<AlpsKnowledgeBroker> m_broker;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;

    static char to_mibs_type(VarType t_type);

    void load_auxiliary_data();
    std::pair<std::vector<int>, std::vector<int>> dispatch_variable_indices();
    std::pair<std::vector<int>, std::vector<int>> dispatch_constraint_indices();
    std::vector<double> find_lower_level_objective_coefficients(const std::vector<int>& t_lower_level_variables_indices);
    std::pair<std::vector<double>, std::vector<double>> find_lower_level_bounds(const std::vector<int>& t_lower_level_variables_indices);

    void load_problem_data();
    std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> parse_variables();
    std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> parse_constraints();
    CoinPackedMatrix parse_matrix();
    std::vector<double> parse_objective();
public:
    MibSFromAPI(const idol::Model& t_model,
                const idol::Bilevel::LowerLevelDescription& t_description,
                OsiSolverInterface* t_osi_solver,
                bool t_logs);

    void solve() override;

    double get_best_obj() const override;

    double get_best_bound() const override;

    double get_var_primal(const Var& t_var) const override;

    idol::SolutionStatus get_status() const override;

    idol::SolutionReason get_reason() const override;

};

#endif

#endif //IDOL_IMPL_MIBS_FROM_API_H
