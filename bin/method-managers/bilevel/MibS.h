//
// Created by Henri on 20/05/2026.
//

#ifndef IDOL_BILEVELMETHODS_MIBS_H
#define IDOL_BILEVELMETHODS_MIBS_H

#include "BilevelMethodManager.h"

namespace BilevelMethods {
    class MibS;
}

class BilevelMethods::MibS : public BilevelMethod {
public:
    [[nodiscard]] unsigned score() const override { return 1000; }
    [[nodiscard]] std::string name() const override { return "MibS"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const BilevelMethodManager& t_parent) const override;
};

#endif //IDOL_BILEVELMETHODS_MIBS_H