//
// Created by Henri on 19/05/2026.
//

#include "Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"

std::string MILPMethods::Cplex::description() const {
    return "IBM ILOG CPLEX Optimization Studio [https://www.ibm.com/fr-fr/products/ilog-cplex-optimization-studio]";
}

std::vector<MILPMethod::Conditions> MILPMethods::Cplex::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_cplex = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::Cplex::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    auto cplex = idol::Cplex();
    cplex.with_continuous_relaxation_only(t_continuous_relaxation);

    result.reset(cplex.clone());

    return result;
}