//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_IMPL_CONSTRAINT_H
#define OPTIMIZE_IMPL_CONSTRAINT_H

#include "../objects/impl_Object.h"
#include "../Types.h"
#include "../expressions/Row.h"

namespace impl {
    class Ctr;
}

class TempCtr;

class impl::Ctr : public impl::Object {
    CtrType m_type;
    Row m_row;
public:
    Ctr(ObjectId&& t_id, CtrType t_type, Constant&& t_rhs);
    Ctr(ObjectId&& t_id, TempCtr&& t_temporary_constraint);

    [[nodiscard]] CtrType type() const;

    void set_type(CtrType t_type) { m_type = t_type; }

    Row& row();

    const Row& row() const;
};

#endif //OPTIMIZE_IMPL_CONSTRAINT_H
