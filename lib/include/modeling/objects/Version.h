//
// Created by henri on 30/01/23.
//

#ifndef IDOL_VERSION_H
#define IDOL_VERSION_H

class Version {
    unsigned int m_index;
public:
    explicit Version(unsigned int t_index) : m_index(t_index) {}

    [[nodiscard]] unsigned int index() const { return m_index; }

    void set_index(unsigned int t_index) { m_index = t_index; }
};

#endif //IDOL_VERSION_H
