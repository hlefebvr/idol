//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_MILPMETHODS_GLPK_H
#define IDOL_MILPMETHODS_GLPK_H

#include "MILPMethodManager.h"

namespace MILPMethods {
    class GLPK;
}

class MILPMethods::GLPK : public MILPMethod {
public:
    [[nodiscard]] unsigned score() const override { return 500; }
    [[nodiscard]] std::string name() const override { return "GLPK"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const override;
};

#endif //IDOL_MILPMETHODS_GLPK_H