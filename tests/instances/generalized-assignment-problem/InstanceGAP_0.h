//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_INSTANCEGAP_0_H
#define OPTIMIZE_INSTANCEGAP_0_H

#include "AbstractInstanceGAP.h"

template<unsigned int N> struct InstanceGAP;

// Example 7.3 from http://www.or.deis.unibo.it/kp/Chapter7.pdf

template<>
struct InstanceGAP<0> : public AbstractInstanceGAP {

    [[nodiscard]] unsigned int n_knapsacks() const override {
        return 3;
    }

    [[nodiscard]] unsigned int n_items() const override {
        return 8;
    }

    [[nodiscard]] std::vector<std::vector<double>> p() const override {
        return {
                { -27, -12, -12, -16, -24, -31, -41, -13 },
                { -14,  -5, -37,  -9, -36, -25,  -1, -35 },
                { -34, -34, -20,  -9, -19, -19,  -3, -24 }
        };
    }

    [[nodiscard]] std::vector<std::vector<double>> w() const override {
        return {
                { 21, 13,  9,  5,  7, 15,  5, 24 },
                { 20,  8, 18, 25,  6,  6,  9,  6 },
                { 16, 16, 18, 24, 11, 11, 16, 18 }
        };
    }

    [[nodiscard]] std::vector<double> c() const override {
        return {
                26, 25, 34
        };
    }

    [[nodiscard]] double optimum() const override { return -233; }

};

#endif //OPTIMIZE_INSTANCEGAP_0_H
