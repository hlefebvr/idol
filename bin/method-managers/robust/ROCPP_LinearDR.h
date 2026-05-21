//
// Created by Henri on 21/05/2026.
//

#ifndef IDOL_ROCPP_PIECEWISELINEARDR_H
#define IDOL_ROCPP_PIECEWISELINEARDR_H

#include "RobustMethodManager.h"

namespace RobustMethods {
    class ROCPP_LinearDR;
}

class RobustMethods::ROCPP_LinearDR : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 50; }
    [[nodiscard]] std::string name() const override { return "ROCPP_LinearDR"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_ROCPP_PIECEWISELINEARDR_H