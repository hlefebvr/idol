//
// Created by henri on 12/10/22.
//

#ifndef IDOL_KP_INSTANCE_H
#define IDOL_KP_INSTANCE_H

#include <vector>
#include <string>

namespace Problems::KP {
    class Instance;
    Instance read_instance(const std::string& t_filename);
}

class Problems::KP::Instance {
protected:
    std::vector<double> m_profit;
    std::vector<double> m_weight;
    double m_capacity = 0;
public:
    explicit Instance(unsigned int t_n_items);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_items() const { return m_profit.size(); }
    [[nodiscard]] double profit(unsigned int t_item) const { return m_profit[t_item]; }
    [[nodiscard]] double weight(unsigned int t_item) const { return m_weight[t_item]; }
    [[nodiscard]] double capacity() const { return m_capacity; }

    void set_profit(unsigned int t_item, double t_value) { m_profit[t_item] = t_value; }
    void set_weight(unsigned int t_item, double t_value) { m_weight[t_item] = t_value; }
    void set_capacity(double t_value) { m_capacity = t_value; }
};

#endif //IDOL_KP_INSTANCE_H
