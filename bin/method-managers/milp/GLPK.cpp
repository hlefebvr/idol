//
// Created by Henri on 19/05/2026.
//

#include "GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

std::string MILPMethods::GLPK::description() const {
    return "GLPK (GNU Linear Programming Kit) [https://www.gnu.org/software/glpk/]";
}

std::vector<MILPMethod::Conditions> MILPMethods::GLPK::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_glpk = true;
        conditions.requires_no_sos_constraint = true;
        conditions.requires_no_quadratic_constraint = true;
        conditions.requires_no_quadratic_objective_function = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::GLPK::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    auto glpk = idol::GLPK();
    glpk.with_continuous_relaxation_only(t_continuous_relaxation);

    result.reset(glpk.clone());

    return result;
}
