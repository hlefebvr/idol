//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_IMPL_PARAM_H
#define OPTIMIZE_IMPL_PARAM_H

#include "../objects/impl_Object.h"
#include "../Types.h"

namespace impl {
    class Param;
}

class impl::Param : public impl::Object {
    double m_lb;
    double m_ub;
    VarType m_type;
public:
    Param(ObjectId&& t_id, double t_lb, double t_ub, VarType t_type);

    void set_lb(double t_lb) { m_lb = t_lb; }

    void set_ub(double t_ub) { m_ub = t_ub; }

    void set_type(VarType t_type) { m_type = t_type; }

    [[nodiscard]] double lb() const { return m_lb; }

    [[nodiscard]] double ub() const { return m_ub; }

    [[nodiscard]] VarType type() const { return m_type; }
};

#endif //OPTIMIZE_IMPL_PARAM_H
