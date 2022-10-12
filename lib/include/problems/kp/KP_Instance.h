//
// Created by henri on 12/10/22.
//

#ifndef IDOL_KP_INSTANCE_H
#define IDOL_KP_INSTANCE_H

#include <vector>
#include <string>

namespace ProblemSpecific::KP {
    class Instance;
    Instance read_instance(const std::string& t_filename);
}

class ProblemSpecific::KP::Instance {
protected:
    std::vector<double> m_p;
    std::vector<double> m_w;
    double m_t = 0;
public:
    explicit Instance(unsigned int t_n_items);

    Instance(const Instance&) = default;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = default;
    Instance& operator=(Instance&&) noexcept = default;

    ~Instance() = default;
    [[nodiscard]] unsigned int n_items() const { return m_p.size(); }
    [[nodiscard]] double p(unsigned int t_item) const { return m_p[t_item]; }
    [[nodiscard]] double w(unsigned int t_item) const { return m_w[t_item]; }
    [[nodiscard]] double t() const { return m_t; }

    void set_p(unsigned int t_item, double t_value) { m_p[t_item] = t_value; }
    void set_w(unsigned int t_item, double t_value) { m_w[t_item] = t_value; }
    void set_t(double t_value) { m_t = t_value; }
};

#endif //IDOL_KP_INSTANCE_H
