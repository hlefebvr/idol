//
// Created by henri on 03/11/22.
//

#ifndef IDOL_FLP_INSTANCE_H
#define IDOL_FLP_INSTANCE_H

#include <vector>
#include <string>

namespace Problems::FLP {
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
     */
    Instance read_instance_2021_Cheng_et_al(const std::string& t_filename);

    Instance read_instance_1991_Cornuejols_et_al(const std::string& t_filename);

    Instance generate_instance_1991_Cornuejols_et_al(unsigned int t_n_facilities, unsigned int t_n_customers, double t_ratio_capacity_over_demand);
}

class Problems::FLP::Instance {
    // For each facility
    std::vector<double> m_fixed_costs;
    std::vector<double> m_capacities;

    // For each customer
    std::vector<double> m_demands;
    std::vector<double> m_per_unit_penalties;

    // For each (facility, customer)
    std::vector<std::vector<double>> m_per_unit_transportation_costs;
public:
    explicit Instance(unsigned int t_n_facilities, unsigned int t_n_customers);

    [[nodiscard]] unsigned int n_facilities() const { return m_fixed_costs.size(); }
    [[nodiscard]] unsigned int n_customers() const { return m_demands.size(); }

    [[nodiscard]] double fixed_cost(unsigned int t_i) const { return m_fixed_costs[t_i]; }
    [[nodiscard]] double capacity(unsigned int t_i) const { return m_capacities[t_i]; }
    [[nodiscard]] double demand(unsigned int t_j) const { return m_demands[t_j]; }
    [[nodiscard]] double per_unit_transportation_cost(unsigned int t_i, unsigned int t_j) const { return m_per_unit_transportation_costs[t_i][t_j]; }

    void set_fixed_cost(unsigned int t_i, double t_value) { m_fixed_costs[t_i] = t_value; }
    void set_capacity(unsigned int t_i, double t_value) { m_capacities[t_i] = t_value; }
    void set_demand(unsigned int t_j, double t_value) { m_demands[t_j] = t_value; }
    void set_per_unit_transportation_cost(unsigned int t_i, unsigned int t_j, double t_value) { m_per_unit_transportation_costs[t_i][t_j] = t_value; }

};

std::ostream& operator<<(std::ostream& t_os, const Problems::FLP::Instance& t_instance);

#endif //IDOL_FLP_INSTANCE_H
