//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_CG_NO_GOOD_H
#define IDOL_CG_NO_GOOD_H
#include "RobustMethodManager.h"

namespace RobustMethods {
    class CG_Indicator;
}

class RobustMethods::CG_Indicator : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 100; }
    [[nodiscard]] std::string name() const override { return "CG_Indicator"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_CG_NO_GOOD_H