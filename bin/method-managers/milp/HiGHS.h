//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_MILP_METHODS_HIGHS_H
#define IDOL_MILP_METHODS_HIGHS_H

#include "MILPMethodManager.h"

namespace MILPMethods {
    class HiGHS;
}

class MILPMethods::HiGHS : public MILPMethod {
public:
    [[nodiscard]] unsigned score() const override { return 200; }
    [[nodiscard]] std::string name() const override { return "HiGHS"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const override;
};

#endif //IDOL_MILP_METHODS_HIGHS_H