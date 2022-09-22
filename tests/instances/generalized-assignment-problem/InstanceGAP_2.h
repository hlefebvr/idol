//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_INSTANCEGAP_2_H
#define OPTIMIZE_INSTANCEGAP_2_H

#include "AbstractInstanceGAP.h"

template<unsigned int N> struct InstanceGAP;

template<>
struct InstanceGAP<2> : public AbstractInstanceGAP {

    [[nodiscard]] unsigned int n_knapsacks() const override {
        return 2;
    }

    [[nodiscard]] unsigned int n_items() const override {
        return 7;
    }

    [[nodiscard]] std::vector<std::vector<double>> p() const override {
        return {
                { -6, -9, -4, -2, -10, -3, -6 },
                { -4, -8, -9, -1,  -7, -5, -4 }
        };
    }

    [[nodiscard]] std::vector<std::vector<double>> w() const override {
        return {
                { 4, 1, 2, 1, 4, 3, 8 },
                { 9, 9, 8, 1, 3, 8, 7 }
        };
    }

    [[nodiscard]] std::vector<double> c() const override {
        return {
                11, 22
        };
    }

    [[nodiscard]] double optimum() const override { return -40; }

};

#endif //OPTIMIZE_INSTANCEGAP_2_H
