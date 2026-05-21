//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_MILPMETHODS_JUMP_H
#define IDOL_MILPMETHODS_JUMP_H

#include "MILPMethodManager.h"

namespace MILPMethods {
    class JuMP;
}

class MILPMethods::JuMP : public MILPMethod {
public:
    [[nodiscard]] unsigned score() const override { return 100; }
    [[nodiscard]] std::string name() const override { return "JuMP"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const override;
};

#endif //IDOL_MILPMETHODS_JUMP_H