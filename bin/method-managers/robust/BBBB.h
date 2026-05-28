//
// Created by Henri on 26/05/2026.
//

#ifndef IDOL_BBBB_H
#define IDOL_BBBB_H

#include "RobustMethodManager.h"

namespace RobustMethods {
    class BBBB;
}

class RobustMethods::BBBB : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 300; }
    [[nodiscard]] std::string name() const override { return "BBBB"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_BBBB_H