//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_ROBUSTMETHODMANAGER_H
#define IDOL_ROBUSTMETHODMANAGER_H

#include "../AbstractMethodManager.h"
#include "../../VariableAnalysis.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"

class RobustMethodManager;

struct UncertaintySetAnalysisResult : VariableAnalysisResult{
    bool is_zero_one_polytope = false;
    bool has_integer_linking_variables = true;
    bool has_binary_linking_variables = true;
    bool has_integer_coefficients = true;
    bool has_decision_dependence = false;
    bool has_rhs_uncertainty = false;
    bool has_matrix_uncertainty = false;
};


struct StageAnalysisResult {
    struct FirstStage : VariableAnalysisResult {};
    struct SecondStage : VariableAnalysisResult {};

    FirstStage first_stage;
    SecondStage second_stage;
    bool has_second_stage = false;

};

class RobustMethod : public AbstractMethod {
public:
    struct Conditions {
        // Stage conditions
        bool requires_single_stage_problem = false;
        bool requires_two_stage_problem = false;

        // Uncertainty set conditions
        bool requires_binary_uncertainty = false;
        bool requires_general_integer_uncertainty_set = false;
        bool requires_continuous_uncertainty_set = false;
        bool requires_a_0_1_uncertainty_set = false;
        bool requires_decision_independent_uncertainty_set = false;
        bool requires_decision_dependent_uncertainty_set = false;
        bool requires_uncertainty_set_with_integer_coefficients = false;
        bool requires_binary_linking_variables_in_uncertainty_set = false;
        bool requires_integer_linking_variables_in_uncertainty_set = false;
        bool requires_objective_uncertainty_only = false;

        // First-stage conditions

        // Second-stage conditions
        bool requires_continuous_second_stage = false;
        bool requires_binary_second_stage = false;
        bool requires_complete_recourse = false;
        bool requires_bounded_second_stage = false;

        // Solver capabilities
        bool requires_a_solver_with_sos1 = false;
        bool requires_mibs = false;
        bool requires_rocpp = false;
    };

    virtual std::vector<Conditions> conditions() = 0;
    virtual void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const = 0;
};

class RobustMethodManager : public AbstractMethodManager<RobustMethod> {
    const idol::Model* m_model = nullptr;
    const idol::Robust::Description* m_robust_description = nullptr;
    const idol::Bilevel::Description* m_bilevel_description = nullptr;

    StageAnalysisResult m_stage_analysis;
    UncertaintySetAnalysisResult m_uncertainty_set_analysis;
    bool m_problem_has_been_set = false;
protected:
    void do_stage_analysis();
    void do_uncertainty_set_analysis();
    void do_method_analysis(const std::vector<RobustMethod*>& methods, bool t_logs) const;
    void print_methods(const std::vector<RobustMethod*>& methods) const override;
public:
    RobustMethodManager(const Arguments& t_args);

    void set_problem(const idol::Model& t_model, const idol::Robust::Description& t_robust_description, const std::optional<idol::Bilevel::Description>& t_bilevel_description);
    [[nodiscard]] const idol::Robust::Description& robust_description() const { return *m_robust_description; }
    [[nodiscard]] const idol::Bilevel::Description& bilevel_description() const { return *m_bilevel_description; }
    [[nodiscard]] const Arguments& args() const { return m_arguments; }

    [[nodiscard]] const UncertaintySetAnalysisResult& uncertainty_analysis() const { return m_uncertainty_set_analysis; }
    [[nodiscard]] const StageAnalysisResult& stage_analysis() const { return m_stage_analysis; }
    void set_optimizer(idol::Model& t_model) const;
};

#endif //IDOL_ROBUSTMETHODMANAGER_H