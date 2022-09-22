//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_ABSTRACTINSTANCEGAP_H
#define OPTIMIZE_ABSTRACTINSTANCEGAP_H

#include <vector>

class AbstractInstanceGAP {
public:
    virtual ~AbstractInstanceGAP() = default;
    [[nodiscard]] virtual unsigned int n_knapsacks() const = 0;
    [[nodiscard]] virtual unsigned int n_items() const = 0;
    [[nodiscard]] virtual std::vector<std::vector<double>> p() const = 0;
    [[nodiscard]] virtual std::vector<std::vector<double>> w() const = 0;
    [[nodiscard]] virtual std::vector<double> c() const = 0;
    [[nodiscard]] virtual double optimum() const = 0;
};

#endif //OPTIMIZE_ABSTRACTINSTANCEGAP_H
