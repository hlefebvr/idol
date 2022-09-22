//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_INSTANCEGAP_1_H
#define OPTIMIZE_INSTANCEGAP_1_H

#include "AbstractInstanceGAP.h"

template<unsigned int N> struct InstanceGAP;


template<>
struct InstanceGAP<1> : public AbstractInstanceGAP {

    [[nodiscard]] unsigned int n_knapsacks() const override {
        return 2;
    }

    [[nodiscard]] unsigned int n_items() const override {
        return 5;
    }

    [[nodiscard]] std::vector<std::vector<double>> p() const override {
        return {
                { -7, -3, -3, -8, -7 },
                { -5, -3, -8, -4, -1 }
        };
    }

    [[nodiscard]] std::vector<std::vector<double>> w() const override {
        return {
                { 8, 2, 8, 9, 1 },
                { 2, 2, 6, 4, 4 }
        };
    }

    [[nodiscard]] std::vector<double> c() const override {
        return {
                11, 7
        };
    }

    [[nodiscard]] double optimum() const override { return -22; }

};

#endif //OPTIMIZE_INSTANCEGAP_1_H
