//
// Created by henri on 01.02.24.
//

#ifndef IDOL_IMPL_MIBS_FROM_API_H
#define IDOL_IMPL_MIBS_FROM_API_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/bilevel/modeling/Description.h"
#include "impl_MibS.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"

namespace idol::impl {
    class MibSFromAPI;
}

class idol::impl::MibSFromAPI : public idol::impl::MibS {
    const idol::Model& m_model;
    const idol::Bilevel::Description& m_description;
    const std::list<std::unique_ptr<Callback>>& m_callbacks;
    const bool m_logs;
    const bool m_use_cplex_for_feasibility;

    void* m_mibs = nullptr; // MibSModel
    void* m_broker = nullptr; // AlpsKnowledgeBroker
    void* m_osi_solver = nullptr; // OsiSolverInterface

    std::vector<std::vector<unsigned int>> m_ctr_indices_in_mibs;
    unsigned int m_n_ctr_in_mibs = 0;

    static char to_mibs_type(VarType t_type);

    void load_auxiliary_data();
    std::pair<std::vector<int>, std::vector<int>> dispatch_variable_indices();
    std::pair<std::vector<int>, std::vector<int>> dispatch_constraint_indices();
    std::vector<double> find_lower_level_objective_coefficients(const std::vector<int>& t_lower_level_variables_indices);
    std::pair<std::vector<double>, std::vector<double>> find_lower_level_bounds(const std::vector<int>& t_lower_level_variables_indices);

    void load_problem_data();
    std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> parse_variables();
    std::tuple<std::vector<double>, std::vector<double>, std::vector<char>> parse_constraints();
    void* parse_matrix(); // returns a CoinPackedMatrix
    std::vector<double> parse_objective();
public:
    MibSFromAPI(const idol::Model& t_model,
                const idol::Bilevel::Description& t_description,
                void* t_osi_solver,
                const std::list<std::unique_ptr<Callback>>& t_callbacks,
                bool t_use_cplex_for_feasibility,
                bool t_logs);

    ~MibSFromAPI() override;

    void solve() override;

    [[nodiscard]] double get_best_obj() const override;

    [[nodiscard]] double get_best_bound() const override;

    [[nodiscard]] double get_var_primal(const Var& t_var) const override;

    [[nodiscard]] idol::SolutionStatus get_status() const override;

    [[nodiscard]] idol::SolutionReason get_reason() const override;

};

#endif //IDOL_IMPL_MIBS_FROM_API_H
