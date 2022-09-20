//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_IMPL_PARAM_H
#define OPTIMIZE_IMPL_PARAM_H

#include "../objects/impl_Object.h"
#include "../Types.h"
#include "modeling/variables/Variable.h"

namespace impl {
    class Param;
}

class impl::Param : public impl::Object {
    ::Var m_variable;
public:
    Param(ObjectId&& t_id, const ::Var& t_variable);

    [[nodiscard]] const ::Var& variable() const { return m_variable; }

    ::Var& variable() { return m_variable; }
};

#endif //OPTIMIZE_IMPL_PARAM_H
