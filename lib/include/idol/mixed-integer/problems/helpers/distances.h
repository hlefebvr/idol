//
// Created by henri on 03/11/22.
//

#ifndef IDOL_DISTANCES_H
#define IDOL_DISTANCES_H

#include <cmath>
#include <numbers>

namespace idol {

/**
 * Computes the Euclidean distance between two points (x,y).
 */
    double euclidean(const std::pair<double, double> &a, const std::pair<double, double> &b) {
        return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
    }

/**
 * Computes the haversine distance between two points (lat, lon).
 * See https://www.geeksforgeeks.org/haversine-formula-to-find-distance-between-two-points-on-a-sphere/
 */
    double haversine(const std::pair<double, double> &t_a, const std::pair<double, double> &t_b) {
        auto [lat1, lon1] = t_a;
        auto [lat2, lon2] = t_b;
        // distance between latitudes
        // and longitudes
        const double dLat = (lat2 - lat1) * M_PI / 180.0;
        const double dLon = (lon2 - lon1) * M_PI / 180.0;

        // convert to radians
        lat1 = (lat1) * M_PI / 180.0;
        lat2 = (lat2) * M_PI / 180.0;

        // apply formulae
        const double a =
                std::pow(std::sin(dLat / 2), 2) + std::pow(std::sin(dLon / 2), 2) * std::cos(lat1) * std::cos(lat2);
        // double rad = 6371;
        const double rad = 3958.755866;
        const double c = 2 * std::asin(std::sqrt(a));

        return rad * c;
    }

}

#endif //IDOL_DISTANCES_H
