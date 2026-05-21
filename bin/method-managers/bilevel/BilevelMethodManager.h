//
// Created by Henri on 20/05/2026.
//

#ifndef IDOL_BILEVELMETHODMANAGER_H
#define IDOL_BILEVELMETHODMANAGER_H

#include "../AbstractMethodManager.h"
#include "../../VariableAnalysis.h"
#include "idol/bilevel/modeling/Description.h"

class BilevelMethodManager;

struct BilevelAnalysisResult {
    struct Leader : VariableAnalysisResult {};
    struct Follower : VariableAnalysisResult {};

    Leader leader;
    Follower follower;
    bool has_coupling_constraints = false;
    bool has_continuous_linking_variables = false;
    bool has_binary_linking_variables = false;
    bool has_general_integer_linking_variables = false;
};

class BilevelMethod : public AbstractMethod {
public:
    struct Conditions {
        bool requires_binary_linking_variables = false;
        bool requires_integer_linking_variables = false;
        bool requires_continuous_follower = false;
        bool requires_big_M_file = false;
        bool requires_MibS = false;
    };

    virtual std::vector<Conditions> conditions() = 0;
    [[nodiscard]] virtual std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const BilevelMethodManager& t_parent) const = 0;
};

class BilevelMethodManager : public AbstractMethodManager<BilevelMethod> {
    const idol::Model* m_model = nullptr;
    const idol::Bilevel::Description* m_bilevel_description = nullptr;
    BilevelAnalysisResult m_analysis;
    bool m_problem_has_been_set = false;
public:
    BilevelMethodManager(const Arguments& t_args);

    [[nodiscard]] const idol::Bilevel::Description& bilevel_description() const { return *m_bilevel_description; }

    void set_problem(const idol::Model& t_model, const idol::Bilevel::Description& t_bilevel_description);
    void set_problem_spec(const BilevelAnalysisResult& t_result);
    [[nodiscard]] const Arguments& args() const { return m_arguments; }

    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer() const;
protected:
    void print_methods(const std::vector<BilevelMethod*>& methods) const override;
    void do_problem_analysis();
    void do_method_analysis(const std::vector<BilevelMethod*>& methods, bool t_logs) const;
};


#endif //IDOL_BILEVELMETHODMANAGER_H