//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_INSTANCE_H
#define OPTIMIZE_INSTANCE_H

#include <vector>
#include <string>

namespace Problems::GAP {
    class Instance;
    Instance read_instance(const std::string& t_filename);
}

/**
 * See also https://en.wikipedia.org/wiki/Generalized_assignment_problem.
 */
class Problems::GAP::Instance {
protected:
    std::vector<std::vector<double>> m_p;
    std::vector<std::vector<double>> m_w;
    std::vector<double> m_t;
public:
    Instance(unsigned int t_n_knapsacks, unsigned int t_n_items);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_knapsacks() const { return m_p.size(); }
    [[nodiscard]] unsigned int n_items() const { return m_p[0].size(); }
    [[nodiscard]] double p(unsigned int t_knapsack, unsigned int t_item) const { return m_p[t_knapsack][t_item]; }
    [[nodiscard]] double w(unsigned int t_knapsack, unsigned int t_item) const { return m_w[t_knapsack][t_item]; }
    [[nodiscard]] double t(unsigned int t_knapsack) const { return m_t[t_knapsack]; }

    void set_p(unsigned int t_knapsack, unsigned int t_item, double t_value) { m_p[t_knapsack][t_item] = t_value; }
    void set_w(unsigned int t_knapsack, unsigned int t_item, double t_value) { m_w[t_knapsack][t_item] = t_value; }
    void set_t(unsigned int t_knapsack, double t_value) { m_t[t_knapsack] = t_value; }
};


#endif //OPTIMIZE_INSTANCE_H
