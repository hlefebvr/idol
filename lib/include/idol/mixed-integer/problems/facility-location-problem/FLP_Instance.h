//
// Created by henri on 03/11/22.
//

#ifndef IDOL_FLP_INSTANCE_H
#define IDOL_FLP_INSTANCE_H

#include <vector>
#include <string>
#include <optional>

namespace idol::Problems::FLP {
    class Instance;

    /**
     * Reads an instance from
      Chun Cheng, Yossiri Adulyasak, Louis-Martin Rousseau,
      Robust facility location under demand uncertainty and facility disruptions,
      Omega,
      Volume 103,
      2021,
      102429,
      ISSN 0305-0483,
      https://doi.org/10.1016/j.omega.2021.102429.
     * @param t_filename the filename
     * @param t_d the value for d, i.e., the penalty is set to the d-th percentile of transportation costs
     * @param t_use_haversine whether to use the haversine distance or the Euclidean distance to compute transportation costs
     * @param t_is_symetric if true, will consider the first max(n_facilities, n_customers) points to create both facilities and customers, otherwise
     * it will read the first n_facilities for the facilities and the next n_customers for the customers.
     */
    Instance read_instance_2021_Cheng_et_al(const std::string& t_filename, double t_d, bool t_use_haversine = false, bool t_is_symetric = true);

    Instance read_instance_1991_Cornuejols_et_al(const std::string& t_filename);

    Instance generate_instance_1991_Cornuejols_et_al(unsigned int t_n_facilities, unsigned int t_n_customers, double t_ratio_capacity_over_demand);
}

class idol::Problems::FLP::Instance {
    // For each facility
    std::vector<double> m_fixed_costs;
    std::vector<double> m_capacities;

    // For each customer
    std::vector<double> m_demands;
    std::optional<std::vector<double>> m_per_unit_penalties;

    // For each (facility, customer)
    std::vector<std::vector<double>> m_per_unit_transportation_costs;
public:
    explicit Instance(unsigned int t_n_facilities, unsigned int t_n_customers);

    [[nodiscard]] unsigned int n_facilities() const { return m_fixed_costs.size(); }
    [[nodiscard]] unsigned int n_customers() const { return m_demands.size(); }

    [[nodiscard]] virtual double fixed_cost(unsigned int t_i) const { return m_fixed_costs[t_i]; }
    [[nodiscard]] virtual double capacity(unsigned int t_i) const { return m_capacities[t_i]; }
    [[nodiscard]] virtual double demand(unsigned int t_j) const { return m_demands[t_j]; }
    [[nodiscard]] virtual double per_unit_transportation_cost(unsigned int t_i, unsigned int t_j) const { return m_per_unit_transportation_costs[t_i][t_j]; }
    [[nodiscard]] virtual bool has_penalties() const { return m_per_unit_penalties.has_value(); }
    [[nodiscard]] virtual double per_unit_penalty(unsigned int t_j) const { return (*m_per_unit_penalties)[t_j]; }

    void set_fixed_cost(unsigned int t_i, double t_value) { m_fixed_costs[t_i] = t_value; }
    void set_capacity(unsigned int t_i, double t_value) { m_capacities[t_i] = t_value; }
    void set_demand(unsigned int t_j, double t_value) { m_demands[t_j] = t_value; }
    void set_per_unit_transportation_cost(unsigned int t_i, unsigned int t_j, double t_value) { m_per_unit_transportation_costs[t_i][t_j] = t_value; }
    void set_per_unit_penalty(unsigned int t_j, double t_value);

};

namespace idol::Problems::FLP {
    std::ostream& operator<<(std::ostream& t_os, const idol::Problems::FLP::Instance& t_instance);
}

#endif //IDOL_FLP_INSTANCE_H
