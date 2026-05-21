//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_CCG_CV_H
#define IDOL_CCG_CV_H
#include "RobustMethodManager.h"

namespace RobustMethods {
    class CCG_CV;
}

class RobustMethods::CCG_CV : public RobustMethod {
public:
    [[nodiscard]] unsigned score() const override { return 400; }
    [[nodiscard]] std::string name() const override { return "CCG_CV"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    void set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const override;
};

#endif //IDOL_CCG_CV_H