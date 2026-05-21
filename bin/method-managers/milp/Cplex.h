//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_MILPMETHODS_CPLEX_H
#define IDOL_MILPMETHODS_CPLEX_H

#include "MILPMethodManager.h"

namespace MILPMethods {
    class Cplex;
}

class MILPMethods::Cplex : public MILPMethod {
public:
    [[nodiscard]] unsigned score() const override { return 900; }
    [[nodiscard]] std::string name() const override { return "Cplex"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const override;
};


#endif //IDOL_MILPMETHODS_CPLEX_H