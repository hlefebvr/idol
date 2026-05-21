//
// Created by Henri on 21/05/2026.
//

#ifndef IDOL_ROCPP_KADAPTABILITY_H
#define IDOL_ROCPP_KADAPTABILITY_H

#include "RobustMethodManager.h"

namespace RobustMethods {
    class ROCPP_KAdaptability;
}

class RobustMethods::ROCPP_KAdaptability : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 50; }
    [[nodiscard]] std::string name() const override { return "ROCPP_KAdaptability"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_ROCPP_KADAPTABILITY_H