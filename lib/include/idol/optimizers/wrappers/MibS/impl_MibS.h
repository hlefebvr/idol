//
// Created by henri on 01.02.24.
//

#ifndef IDOL_IMPL_MIBS_H
#define IDOL_IMPL_MIBS_H

#include "idol/modeling/models/Model.h"

#include "MibSModel.hpp"

namespace idol::impl {
    class MibS;
}

class idol::impl::MibS {
    const idol::Model& m_model;
    const idol::Annotation<idol::Var, unsigned int> m_lower_level_variables;
    const idol::Annotation<idol::Ctr, unsigned int> m_lower_level_constraints;
    const idol::Ctr m_lower_level_objective;

    MibSModel m_mibs;
    std::unique_ptr<AlpsKnowledgeBroker> m_broker;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;

    static char to_mibs_type(VarType t_type);

    void load_auxiliary_data();
    std::pair<std::vector<int>, std::vector<int>> dispatch_variable_indices();
    std::pair<std::vector<int>, std::vector<int>> dispatch_constraint_indices();
    std::vector<double> find_lower_level_objective_coefficients(const std::vector<int>& t_lower_level_variables_indices);

    void load_problem_data();
    std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> parse_variables();
    std::tuple<CoinPackedMatrix, std::vector<double>, std::vector<double>, std::vector<char>> parse_constraints();
    CoinPackedVector to_packed_vector(const idol::LinExpr<idol::Var>& t_lin_expr);
    std::vector<double> parse_objective();
public:
    MibS(const idol::Model& t_model,
         const idol::Annotation<idol::Var, unsigned int>& t_lower_level_variables,
         const idol::Annotation<idol::Ctr, unsigned int>& t_lower_level_constraints,
         idol::Ctr  t_lower_level_objective);

    void solve();

    double get_objective_value() const;

    double get_var_primal(const Var& t_var) const;

    idol::SolutionStatus get_status() const;

    idol::SolutionReason get_reason() const;

};


#endif //IDOL_IMPL_MIBS_H
