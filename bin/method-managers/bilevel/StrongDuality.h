//
// Created by Henri on 20/05/2026.
//

#ifndef IDOL_STRONGDUALITY_H
#define IDOL_STRONGDUALITY_H

#include "BilevelMethodManager.h"

namespace BilevelMethods {
    class StrongDuality;
}

class BilevelMethods::StrongDuality : public BilevelMethod {
public:
    [[nodiscard]] unsigned score() const override { return 100; }
    [[nodiscard]] std::string name() const override { return "StrongDuality"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const BilevelMethodManager& t_parent) const override;
};

#endif //IDOL_STRONGDUALITY_H