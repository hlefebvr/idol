//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_VARIABLEANALYSIS_H
#define IDOL_VARIABLEANALYSIS_H

#include "idol/mixed-integer/modeling/models/Model.h"

struct VariableAnalysisResult {
    bool has_continuous = false;
    bool has_general_integer = false;
    bool has_binary = false;
    bool all_bounded = true;
};

inline void do_variable_analysis(VariableAnalysisResult& t_analysis_result, const idol::Model& t_model, const idol::Var& t_var) {

    const auto type = t_model.get_var_type(t_var);
    const double lb = t_model.get_var_lb(t_var);
    const double ub = t_model.get_var_ub(t_var);

    if (idol::is_neg_inf(lb) || idol::is_pos_inf(ub)) {
        t_analysis_result.all_bounded = false;
    }

    if (type == idol::Continuous) {
        t_analysis_result.has_continuous = true;
    } else if (type == idol::Binary) {
        t_analysis_result.has_binary = true;
    } else if (type == idol::Integer) {
        if (lb >= -.5 && ub <= 1.5) {
            t_analysis_result.has_binary = true;
        } else {
            t_analysis_result.has_general_integer = true;
        }
    }

}

#endif //IDOL_VARIABLEANALYSIS_H