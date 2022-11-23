//
// Created by henri on 23/11/22.
//

#ifndef IDOL_VARATTRIBUTES_H
#define IDOL_VARATTRIBUTES_H

#include "TempVar.h"
#include <limits>

class VarAttributes : public TempVar {
    unsigned int m_id = std::numeric_limits<unsigned int>::max();
    unsigned int m_index = std::numeric_limits<unsigned int>::max();
    std::list<Var>::iterator m_it;
public:
    VarAttributes() = default;
    VarAttributes(unsigned int t_id, unsigned int t_index, TempVar&& t_temp_var)
        : m_id(t_id), m_index(t_index), TempVar(std::move(t_temp_var)) {}

    VarAttributes(const VarAttributes&) = default;
    VarAttributes(VarAttributes&&) noexcept = default;

    VarAttributes& operator=(const VarAttributes&) = default;
    VarAttributes& operator=(VarAttributes&&) noexcept = default;

    unsigned int id() const { return m_id; }

    unsigned int index() const { return m_index; }

    void reset() { *this = VarAttributes(); }

    const std::list<Var>::iterator& iterator() const { return m_it; }

    void set_iterator(std::list<Var>::iterator&& t_it) { m_it = t_it; }
};

#endif //IDOL_VARATTRIBUTES_H
