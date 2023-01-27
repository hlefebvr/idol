//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYVARVERSION_H
#define IDOL_MYVARVERSION_H

class VarVersion {
    unsigned int m_index;
    double m_lower_bound;
    double m_upper_bound;
public:
    VarVersion(unsigned int t_index, double t_lb, double t_ub ) : m_index(t_index), m_lower_bound(t_lb), m_upper_bound(t_ub) {}

    [[nodiscard]] double lb() const { return m_lower_bound; }

    [[nodiscard]] double ub() const { return m_upper_bound; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    void set_index(unsigned int t_index) { m_index = t_index; }
};

#endif //IDOL_MYVARVERSION_H
