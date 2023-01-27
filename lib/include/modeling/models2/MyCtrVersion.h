//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYCTRVERSION_H
#define IDOL_MYCTRVERSION_H

class CtrVersion {
    unsigned int m_index;
public:
    explicit CtrVersion(unsigned int t_index) : m_index(t_index) {}

    [[nodiscard]] unsigned int index() const { return m_index; }

    void set_index(unsigned int t_index) { m_index = t_index; }
};

#endif //IDOL_MYCTRVERSION_H
