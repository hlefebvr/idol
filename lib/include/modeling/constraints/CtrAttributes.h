//
// Created by henri on 23/11/22.
//

#ifndef IDOL_CTRATTRIBUTES_H
#define IDOL_CTRATTRIBUTES_H

#include "TempCtr.h"

class CtrAttributes : public TempCtr {
    unsigned int m_id = std::numeric_limits<unsigned int>::max();
    unsigned int m_index = std::numeric_limits<unsigned int>::max();
    std::list<Ctr>::iterator m_it;
public:
    CtrAttributes() = default;
    CtrAttributes(unsigned int t_id, unsigned int t_index, TempCtr&& t_temp_var)
            : m_id(t_id), m_index(t_index), TempCtr(std::move(t_temp_var)) {}

    CtrAttributes(const CtrAttributes&) = default;
    CtrAttributes(CtrAttributes&&) noexcept = default;

    CtrAttributes& operator=(const CtrAttributes&) = default;
    CtrAttributes& operator=(CtrAttributes&&) noexcept = default;

    unsigned int id() const { return m_id; }

    unsigned int index() const { return m_index; }

    void reset() { *this = CtrAttributes(); }

    const std::list<Ctr>::iterator& iterator() const { return m_it; }

    void set_iterator(std::list<Ctr>::iterator&& t_it) { m_it = t_it; }
};

#endif //IDOL_CTRATTRIBUTES_H
