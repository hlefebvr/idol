//
// Created by henri on 03/11/22.
//

#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/mixed-integer/problems/helpers/parse_delimited.h"
#include "idol/mixed-integer/problems/helpers/distances.h"
#include <random>
#include <algorithm>

idol::Problems::FLP::Instance::Instance(unsigned int t_n_facilities, unsigned int t_n_customers)
        : m_fixed_costs(t_n_facilities),
          m_capacities(t_n_facilities),
          m_demands(t_n_customers),
          m_per_unit_transportation_costs(t_n_facilities, std::vector<double>(t_n_customers)) {

}

void idol::Problems::FLP::Instance::set_per_unit_penalty(unsigned int t_j, double t_value) {
    if (!m_per_unit_penalties.has_value()) {
        m_per_unit_penalties = std::vector<double>(n_customers());
    }
    (*m_per_unit_penalties)[t_j] = t_value;
}

idol::Problems::FLP::Instance idol::Problems::FLP::read_instance_2021_Cheng_et_al(const std::string &t_filename, double t_d, bool t_use_haversine, bool t_is_symetric) {

    auto data = parse_delimited(t_filename, '\t');

    if (data[0][0] != "FacNum") {
        throw Exception("Could not parse_variables instance, missing header FacNum.");
    }

    if (data[0][2] != "CustNum") {
        throw Exception("Could not parse_variables instance, missing header CustNum.");
    }

    unsigned int n_facilities = std::stoul(data[0][1]);
    unsigned int n_customers = std::stoul(data[0][3]);

    Problems::FLP::Instance result(n_facilities, n_customers);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        result.set_fixed_cost(i, std::stod(data[i+1][4]));
        result.set_capacity(i, std::stod(data[i+1][5]));
    }

    unsigned int i_begin = 0, j_begin = 0, i_end = n_facilities, j_end = n_customers;
    if (!t_is_symetric) {
        j_begin = n_facilities;
        j_end = n_facilities + n_customers;
    }

    std::vector<double> costs;
    costs.reserve(n_facilities * n_customers);
    for (unsigned int i = i_begin ; i < i_end ; ++i) {
        for (unsigned int j = j_begin ; j < j_end ; ++j) {
            const double lon_i = std::stod(data[i+1][1]);
            const double lat_i = std::stod(data[i+1][2]);
            const double lon_j = std::stod(data[j+1][1]);
            const double lat_j = std::stod(data[j+1][2]);
            const double distance = t_use_haversine ?
                    std::floor(20 * haversine({lat_i, lon_i}, {lat_j, lon_j}))
                    :
                    std::floor(20 * euclidean({lat_i, lon_i}, {lat_j, lon_j}));
            result.set_per_unit_transportation_cost(i, j, distance);
            costs.emplace_back(distance);
        }
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        const double d_j = std::floor(std::stod(data[j+1][3]) * 1e-5);
        result.set_demand(j, d_j);
    }

    std::sort(costs.begin(), costs.end());
    const unsigned int index = std::max<unsigned int>(1, std::ceil(t_d * n_facilities * n_customers)) - 1;
    const double penalty = costs[index];
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        result.set_per_unit_penalty(j, penalty);
    }

    return result;
}

idol::Problems::FLP::Instance idol::Problems::FLP::read_instance_1991_Cornuejols_et_al(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw Exception("Could not open instance file.");
    }

    unsigned int n_facilities, n_customers;

    file >> n_facilities >> n_customers;

    Problems::FLP::Instance result(n_facilities, n_customers);

    double placeholder;

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        file >> placeholder;
        result.set_fixed_cost(i, placeholder);
        file >> placeholder;
        result.set_capacity(i, placeholder);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        file >> placeholder;
        result.set_demand(j, placeholder);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            file >> placeholder;
            result.set_per_unit_transportation_cost(i, j, placeholder);
        }
    }

    return result;
}

std::ostream& idol::Problems::FLP::operator<<(std::ostream& t_os, const idol::Problems::FLP::Instance& t_instance) {
    t_os << t_instance.n_facilities() << '\t' << t_instance.n_customers() << '\n';
    for (unsigned int i = 0, n = t_instance.n_facilities() ; i < n ; ++i) {
        t_os << t_instance.fixed_cost(i) << '\t' << t_instance.capacity(i) << '\n';
    }
    for (unsigned int j = 0, m = t_instance.n_customers() ; j < m ; ++j) {
        t_os << t_instance.demand(j) << '\n';
    }
    for (unsigned int i = 0, n = t_instance.n_facilities() ; i < n ; ++i) {
        for (unsigned int j = 0, m = t_instance.n_customers() ; j < m ; ++j) {
            t_os << t_instance.per_unit_transportation_cost(i, j) << '\t';
        }
        t_os << '\n';
    }
    return t_os;
}

idol::Problems::FLP::Instance idol::Problems::FLP::generate_instance_1991_Cornuejols_et_al(unsigned int t_n_facilities, unsigned int t_n_customers, double t_ratio_capacity_over_demand) {

    std::random_device rd;
    std::mt19937 engine(rd());

    const auto generate_random_xy = [&](unsigned int n) {
        std::vector<std::pair<double, double>> result;
        result.reserve(result.size() + n);
        for (unsigned int i = 0 ; i < n ; ++i) {
            result.emplace_back(
                    std::uniform_real_distribution<double>(0, 1)(engine),
                    std::uniform_real_distribution<double>(0, 1)(engine)
            );
        }
        return result;
    };

    Problems::FLP::Instance result(t_n_facilities, t_n_customers);

    auto facility_locations = generate_random_xy(t_n_facilities);
    auto customer_locations = generate_random_xy(t_n_customers);

    const double fixed_cost_scaling_factor = (t_n_facilities * t_n_customers < 500) ? 2.0 : 1.0;

    // Capacities and Fixed costs
    double sum_capacities = 0;
    for (unsigned int i = 0 ; i < t_n_facilities ; ++i) {
        const double capacity = std::uniform_real_distribution<double>(10, 160)(engine);
        const double fixed_cost = fixed_cost_scaling_factor * std::uniform_real_distribution<double>(0, 90)(engine);
        sum_capacities += capacity;
        result.set_capacity(i, std::round(capacity));
        result.set_fixed_cost(i, std::round(fixed_cost));
    }

    // Demands
    double sum_unscaled_demands = 0;
    for (unsigned int j = 0 ; j < t_n_customers ; ++j) {
        const double demand = std::uniform_real_distribution<double>(0, 1)(engine);
        sum_unscaled_demands += demand;
        result.set_demand(j, demand);
    }

    const double demand_scaling_factor = sum_capacities / (t_ratio_capacity_over_demand * sum_unscaled_demands);

    for (unsigned int j = 0 ; j < t_n_customers ; ++j) {
        const double scaled_demand = demand_scaling_factor * result.demand(j);
        result.set_demand(j, std::round(scaled_demand));
    }

    // Per unit transportation costs
    for (unsigned int i = 0, n = t_n_facilities ; i < n ; ++i) {
        for (unsigned int j = 0, m = t_n_customers ; j < m ; ++j) {
            const double distance = euclidean(facility_locations[i], customer_locations[j]);
            result.set_per_unit_transportation_cost(i, j, std::round(distance * 1e2) / 1e2);
        }
    }

    return result;
}
