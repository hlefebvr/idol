//
// Created by henri on 12/10/22.
//

#ifndef IDOL_MKP_INSTANCE_H
#define IDOL_MKP_INSTANCE_H

#include <vector>
#include <string>

namespace idol::Problems::MKP {
    class Instance;
    Instance read_instance(const std::string& t_filename);
}

/**
 * See also https://en.wikipedia.org/wiki/Knapsack_problem.
 */
class idol::Problems::MKP::Instance {
protected:
    std::vector<double> m_profits;
    std::vector<double> m_weights;
    std::vector<double> m_capacities;
public:
    Instance(unsigned int t_n_knapsacks, unsigned int t_n_items);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_knapsacks() const { return m_capacities.size(); }
    [[nodiscard]] unsigned int n_items() const { return m_profits.size(); }
    [[nodiscard]] double profit(unsigned int t_item) const { return m_profits[t_item]; }
    [[nodiscard]] double weight(unsigned int t_item) const { return m_weights[t_item]; }
    [[nodiscard]] double capacity(unsigned int t_knapsack) const { return m_capacities[t_knapsack]; }

    [[nodiscard]] const auto& profits() const { return m_profits; }
    [[nodiscard]] const auto& weights() const { return m_weights; }
    [[nodiscard]] const auto& capacities() const { return m_capacities; }

    void set_profit(unsigned int t_item, double t_value) { m_profits[t_item] = t_value; }
    void set_weight(unsigned int t_item, double t_value) { m_weights[t_item] = t_value; }
    void set_capacity(unsigned int t_knapsack, double t_value) { m_capacities[t_knapsack] = t_value; }

};

#endif //IDOL_MKP_INSTANCE_H
