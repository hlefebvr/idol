//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_INSTANCE_H
#define OPTIMIZE_INSTANCE_H

#include <vector>
#include <string>

namespace idol::Problems::GAP {
    class Instance;
    Instance read_instance(const std::string& t_filename);

    Instance generate_instance_Chu_and_Beasley_1997_C(unsigned int t_n_machines, unsigned int t_n_jobs);
}

/**
 * See also https://en.wikipedia.org/wiki/Generalized_assignment_problem.
 */
class idol::Problems::GAP::Instance {
protected:
    std::vector<std::vector<double>> m_costs;
    std::vector<std::vector<double>> m_resource_consumptions;
    std::vector<double> m_capacities;
public:
    Instance(unsigned int t_n_agents, unsigned int t_n_jobs);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_agents() const { return m_costs.size(); }
    [[nodiscard]] unsigned int n_jobs() const { return m_costs[0].size(); }
    [[nodiscard]] double cost(unsigned int t_agent, unsigned int t_job) const { return m_costs[t_agent][t_job]; }
    [[nodiscard]] double resource_consumption(unsigned int t_agent, unsigned int t_job) const { return m_resource_consumptions[t_agent][t_job]; }
    [[nodiscard]] double capacity(unsigned int t_agent) const { return m_capacities[t_agent]; }

    void set_cost(unsigned int t_agent, unsigned int t_job, double t_value) { m_costs[t_agent][t_job] = t_value; }
    void set_resource_consumption(unsigned int t_agent, unsigned int t_job, double t_value) { m_resource_consumptions[t_agent][t_job] = t_value; }
    void set_capacity(unsigned int t_agent, double t_value) { m_capacities[t_agent] = t_value; }
};

std::ostream& operator<<(std::ostream& t_os, const idol::Problems::GAP::Instance& t_instance);

#endif //OPTIMIZE_INSTANCE_H
