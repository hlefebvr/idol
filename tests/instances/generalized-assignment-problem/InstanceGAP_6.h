//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_INSTANCEGAP_6_H
#define OPTIMIZE_INSTANCEGAP_6_H

#include "AbstractInstanceGAP.h"

template<unsigned int N> struct InstanceGAP;

template<>
struct InstanceGAP<6> : public AbstractInstanceGAP {

    [[nodiscard]] unsigned int n_knapsacks() const override {
        return 5;
    }

    [[nodiscard]] unsigned int n_items() const override {
        return 100;
    }

    [[nodiscard]] std::vector<std::vector<double>> p() const override {
        return {
                { 26, 35, 13, 21, 21, 12, 29, 19, 30, 50, 11, 46, 34, 14, 29, 30, 34, 28, 21, 29, 16, 15, 10, 14, 33, 19, 30, 38, 14, 42, 20, 17, 47, 35, 13, 21, 10, 25, 17, 41, 29, 24, 26, 36, 26, 20, 19, 42, 40, 49, 24, 16, 35, 10, 46, 42, 21, 50, 29, 34, 13, 37, 16, 41, 41, 10, 23, 17, 41, 36, 45, 40, 15, 31, 16, 33, 24, 35, 41, 15, 26, 44, 15, 45, 27, 19, 46, 18, 32, 41, 42, 26, 25, 43, 15, 20, 12, 12, 50, 18, },
                { 24, 43, 25, 36, 39, 49, 14, 42, 40, 16, 50, 43, 38, 30, 40, 25, 18, 31, 32, 15, 14, 35, 35, 17, 38, 30, 17, 34, 50, 21, 13, 37, 16, 28, 49, 27, 32, 24, 45, 30, 33, 32, 10, 31, 45, 31, 14, 27, 16, 27, 41, 40, 45, 44, 50, 14, 19, 41, 36, 21, 43, 25, 30, 42, 20, 48, 17, 34, 12, 35, 46, 39, 23, 18, 27, 28, 19, 41, 12, 39, 12, 13, 42, 39, 42, 50, 41, 19, 42, 26, 21, 17, 46, 23, 48, 40, 17, 23, 49, 10, },
                { 40, 45, 27, 33, 16, 19, 13, 20, 19, 21, 17, 48, 24, 28, 18, 19, 49, 39, 38, 35, 44, 45, 12, 39, 39, 21, 49, 38, 15, 26, 32, 12, 19, 27, 38, 12, 40, 29, 17, 11, 24, 23, 50, 40, 10, 16, 47, 45, 31, 12, 23, 20, 25, 23, 27, 40, 44, 34, 48, 43, 48, 37, 31, 34, 19, 45, 35, 17, 27, 30, 21, 27, 17, 25, 42, 40, 35, 34, 19, 39, 39, 14, 13, 17, 10, 40, 13, 34, 46, 46, 16, 17, 49, 23, 45, 49, 32, 49, 12, 47, },
                { 14, 31, 22, 29, 28, 17, 40, 31, 24, 42, 39, 23, 47, 22, 17, 22, 48, 40, 14, 11, 36, 50, 37, 44, 16, 23, 43, 47, 12, 38, 37, 18, 22, 34, 30, 38, 48, 35, 45, 37, 50, 32, 24, 40, 32, 19, 40, 32, 49, 36, 47, 24, 47, 46, 27, 46, 15, 22, 11, 39, 40, 22, 50, 49, 12, 27, 28, 14, 32, 45, 10, 37, 46, 27, 28, 28, 15, 38, 38, 44, 29, 31, 21, 20, 36, 45, 18, 22, 10, 37, 30, 14, 39, 19, 24, 12, 14, 26, 13, 15, },
                { 19, 20, 45, 11, 10, 35, 37, 25, 23, 41, 44, 10, 27, 18, 22, 49, 45, 26, 34, 12, 33, 30, 30, 20, 15, 13, 49, 23, 38, 38, 37, 41, 12, 26, 50, 20, 47, 13, 48, 50, 44, 38, 42, 46, 27, 16, 27, 16, 38, 16, 13, 14, 16, 34, 39, 44, 19, 41, 47, 34, 39, 17, 44, 24, 19, 14, 40, 38, 40, 31, 37, 16, 29, 50, 50, 22, 36, 36, 29, 49, 23, 13, 49, 25, 40, 30, 16, 50, 27, 25, 11, 36, 36, 28, 33, 43, 50, 24, 24, 43, },
        };
    }

    [[nodiscard]] std::vector<std::vector<double>> w() const override {
        return {
                { 7, 16, 18, 15, 17, 10, 17, 11, 19, 6, 5, 19, 21, 19, 23, 12, 7, 5, 21, 20, 23, 12, 22, 6, 14, 17, 24, 25, 22, 8, 13, 7, 24, 11, 8, 22, 15, 24, 15, 18, 23, 11, 17, 16, 6, 21, 21, 9, 24, 9, 22, 9, 25, 6, 8, 8, 16, 12, 24, 6, 12, 13, 18, 18, 8, 20, 22, 5, 15, 13, 9, 11, 11, 11, 8, 7, 5, 14, 19, 12, 17, 9, 16, 18, 12, 6, 10, 5, 18, 21, 24, 9, 9, 19, 5, 8, 18, 8, 25, 21, },
                { 9, 17, 13, 18, 7, 14, 22, 16, 6, 10, 13, 15, 10, 25, 12, 13, 18, 15, 19, 14, 17, 25, 19, 24, 11, 10, 19, 13, 20, 23, 9, 13, 13, 17, 14, 22, 24, 19, 25, 17, 15, 5, 12, 10, 19, 23, 7, 17, 19, 15, 21, 6, 25, 20, 10, 7, 15, 16, 17, 23, 17, 19, 18, 16, 22, 20, 19, 7, 8, 19, 12, 13, 21, 20, 25, 9, 9, 11, 7, 15, 12, 15, 20, 18, 22, 18, 5, 11, 22, 7, 16, 9, 21, 10, 18, 11, 15, 17, 10, 20, },
                { 18, 24, 6, 18, 16, 18, 15, 23, 19, 9, 21, 7, 17, 9, 19, 22, 23, 5, 21, 9, 14, 21, 9, 23, 15, 24, 22, 12, 12, 24, 6, 20, 23, 15, 15, 14, 12, 9, 24, 20, 19, 9, 7, 20, 13, 14, 14, 10, 13, 21, 18, 12, 16, 19, 5, 14, 17, 20, 13, 22, 16, 19, 5, 11, 25, 10, 13, 13, 6, 6, 9, 13, 10, 22, 10, 9, 5, 13, 21, 16, 11, 20, 5, 13, 12, 5, 11, 11, 14, 14, 18, 8, 9, 8, 20, 15, 17, 21, 25, 15, },
                { 14, 7, 7, 9, 9, 7, 23, 21, 19, 11, 10, 9, 5, 11, 19, 20, 12, 12, 10, 25, 6, 17, 12, 16, 19, 20, 19, 12, 5, 14, 17, 11, 15, 14, 9, 18, 20, 25, 21, 11, 18, 20, 13, 23, 18, 17, 21, 6, 5, 12, 24, 22, 6, 12, 17, 11, 12, 5, 11, 25, 6, 8, 7, 21, 9, 13, 25, 5, 20, 24, 11, 25, 14, 14, 13, 5, 22, 15, 22, 18, 23, 5, 7, 9, 17, 18, 23, 16, 19, 24, 21, 11, 10, 20, 19, 14, 19, 16, 17, 22, },
                { 21, 11, 6, 14, 16, 12, 12, 6, 22, 18, 16, 8, 18, 13, 16, 16, 13, 8, 23, 18, 11, 15, 6, 11, 17, 17, 20, 8, 9, 17, 11, 11, 22, 6, 24, 24, 17, 20, 11, 16, 15, 11, 9, 21, 6, 24, 21, 6, 25, 9, 21, 15, 21, 6, 8, 23, 21, 23, 7, 12, 7, 17, 25, 21, 18, 5, 11, 6, 10, 24, 25, 22, 11, 16, 9, 7, 5, 8, 9, 7, 8, 24, 6, 15, 24, 7, 19, 25, 5, 16, 6, 23, 20, 18, 15, 15, 14, 15, 20, 25, },
        };
    }

    [[nodiscard]] std::vector<double> c() const override {
        return {
                209, 209, 209, 209, 209,
        };
    }

    [[nodiscard]] double optimum() const override { return 1843; }

};

#endif //OPTIMIZE_INSTANCEGAP_6_H
