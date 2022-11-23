//
// Created by henri on 23/11/22.
//

#ifndef IDOL_VARATTRIBUTES_H
#define IDOL_VARATTRIBUTES_H

#include "TempVar.h"
#include <limits>

class VarAttributes : public TempVar {
    unsigned int m_id = std::numeric_limits<unsigned int>::max();
    std::list<Var>::iterator m_it;
public:
    VarAttributes() = default;
    template<class ...Args> VarAttributes(unsigned int t_id, std::list<Var>::iterator&& t_it, Args&& ...t_args)
        : m_id(t_id), m_it(t_it), TempVar(std::forward<Args>(t_args)...) {}

    VarAttributes(const VarAttributes&) = default;
    VarAttributes(VarAttributes&&) noexcept = default;

    VarAttributes& operator=(const VarAttributes&) = default;
    VarAttributes& operator=(VarAttributes&&) noexcept = default;

    unsigned int id() const { return m_id; }

    void reset() { *this = VarAttributes(); }

    const std::list<Var>::iterator& iterator() const { return m_it; }

    bool is_placeholder() const { return m_id == std::numeric_limits<unsigned int>::max(); }
};

#endif //IDOL_VARATTRIBUTES_H
