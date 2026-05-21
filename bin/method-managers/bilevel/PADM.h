//
// Created by Henri on 20/05/2026.
//

#ifndef IDOL_BILEVELMETHODS_PADM_H
#define IDOL_BILEVELMETHODS_PADM_H

#include "BilevelMethodManager.h"

namespace BilevelMethods {
    class PADM;
}

class BilevelMethods::PADM : public BilevelMethod {
public:
    [[nodiscard]] unsigned score() const override { return 10; }
    [[nodiscard]] std::string name() const override { return "PADM"; }
    [[nodiscard]] std::string description() const override;
    std::vector<Conditions> conditions() override;
    [[nodiscard]] std::unique_ptr<idol::OptimizerFactory> get_optimizer_factory(const BilevelMethodManager& t_parent) const override;
};

#endif //IDOL_BILEVELMETHODS_PADM_H