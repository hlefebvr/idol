//
// Created by Henri on 22/04/2026.
//

#ifndef IDOL_COUNTERFACTUAL_H
#define IDOL_COUNTERFACTUAL_H

#include <string>
#include <vector>

#include "idol/bilevel/modeling/Description.h"

namespace idol::Problems::Counterfactual {
    class Instance;
}

namespace  idol::Bilevel {
    Description make_weak_CE_bilevel_model(Model& t_model, const Problems::Counterfactual::Instance& t_instance);
};

class idol::Problems::Counterfactual::Instance {
public:
    Instance(std::string  t_CE_filename, std::string  t_mps_filename);

    struct DesiredSpec {
        unsigned int column = -1;
        double lb = 0.0;
        double ub = 0.0;
    };

    struct MutableCost {
        unsigned int column = -1;
    };

    struct MutableCoefficient {
        unsigned int row = -1;
        unsigned int column = -1;
    };

    struct MutableRhs {
        unsigned int row = -1;
    };

    [[nodiscard]] const std::vector<DesiredSpec>& desired_space() const { return m_desired_space; }

    [[nodiscard]] const std::vector<MutableCost>& mutable_costs() const { return m_mutable_costs; }

    [[nodiscard]] const std::vector<MutableCoefficient>& mutable_coefficients() const { return m_mutable_coefficients; }

    [[nodiscard]] const std::vector<MutableRhs>& mutable_rhs() const { return m_mutable_rhs; }

    [[nodiscard]] const std::string& CE_path() const { return m_CE_filename; }

    [[nodiscard]] const std::string& mps_path() const { return m_mps_filename; }
private:
    const std::string m_CE_filename;
    const std::string m_mps_filename;

    std::vector<DesiredSpec> m_desired_space;
    std::vector<MutableCost> m_mutable_costs;
    std::vector<MutableCoefficient> m_mutable_coefficients;
    std::vector<MutableRhs> m_mutable_rhs;

    void parse();
};

std::ostream& operator<<(std::ostream& t_os, const idol::Problems::Counterfactual::Instance& t_instance);

#endif //IDOL_COUNTERFACTUAL_H