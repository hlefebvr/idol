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
 * See also https://en.wikipedia.org/wiki/Generalized_assignment_problem.
 */
class idol::Problems::MKP::Instance {
protected:
    std::vector<double> m_p;
    std::vector<double> m_w;
    std::vector<double> m_t;
public:
    Instance(unsigned int t_n_knapsacks, unsigned int t_n_items);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_knapsacks() const { return m_t.size(); }
    [[nodiscard]] unsigned int n_items() const { return m_p.size(); }
    [[nodiscard]] double p(unsigned int t_item) const { return m_p[t_item]; }
    [[nodiscard]] double w(unsigned int t_item) const { return m_w[t_item]; }
    [[nodiscard]] double t(unsigned int t_knapsack) const { return m_t[t_knapsack]; }

    void set_p(unsigned int t_item, double t_value) { m_p[t_item] = t_value; }
    void set_w(unsigned int t_item, double t_value) { m_w[t_item] = t_value; }
    void set_t(unsigned int t_knapsack, double t_value) { m_t[t_knapsack] = t_value; }

};

#endif //IDOL_MKP_INSTANCE_H
