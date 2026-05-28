//
// Created by Henri on 22/05/2026.
//

#ifndef IDOL_BP_OBJ_H
#define IDOL_BP_OBJ_H

#include "RobustMethodManager.h"

namespace RobustMethods {
    class Convexification;
}

class RobustMethods::Convexification : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 50000; }
    [[nodiscard]] std::string name() const override { return "Convexification"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_BP_OBJ_H