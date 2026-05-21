//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_MILPMETHODMANAGER_H
#define IDOL_MILPMETHODMANAGER_H

#include "../AbstractMethodManager.h"
#include "../../VariableAnalysis.h"

class Arguments;
class MILPMethodManager;

struct MILPAnalysisResult : VariableAnalysisResult {
    bool has_quadratic_constraints = false;
    bool has_sos_constraints = false;
    bool has_quadratic_objective_function = false;
};

class MILPMethod : public AbstractMethod {
public:
    struct Conditions {
        bool requires_gurobi = false;
        bool requires_cplex = false;
        bool requires_highs = false;
        bool requires_glpk = false;
        bool requires_jump = false;
        bool requires_jump_optimizer = false;

        bool requires_no_sos_constraint = false;
        bool requires_no_quadratic_constraint = false;
        bool requires_no_quadratic_objective_function = false;
    };

    virtual std::vector<Conditions> conditions() = 0;
    [[nodiscard]] virtual std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const MILPMethodManager& t_parent, bool t_relax_integrality) const = 0;
};

class MILPMethodManager : public AbstractMethodManager<MILPMethod> {
    const idol::Model* m_model = nullptr;
    MILPAnalysisResult m_analysis;
    bool m_problem_has_been_set = false;
public:
    MILPMethodManager(const Arguments& t_args);

    void set_problem(const idol::Model& t_model);
    void set_problem_spec(const MILPAnalysisResult& t_result);
    [[nodiscard]] const Arguments& args() const { return m_arguments; }

    void set_optimizer(idol::Model& t_model) const;
    static std::unique_ptr<idol::OptimizerFactory> get_sub_milp_optimizer(const Arguments& t_args, bool t_relax_integrality = false, const MILPAnalysisResult& t_analysis = MILPAnalysisResult());
protected:
    void print_methods(const std::vector<MILPMethod*>& methods) const override;
    void do_problem_analysis();
    void do_method_analysis(const std::vector<MILPMethod*>& methods, bool t_logs) const;
};

#endif //IDOL_MILPMETHODMANAGER_H
