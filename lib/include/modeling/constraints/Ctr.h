//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "../Types.h"
#include <string>
#include <iostream>
#include <memory>

class Row;
class Constant;
class Var;
class Model;

namespace Solution {
    class Primal;
}

/**
 * Constraint object.
 *
 * A constraint always belong to a single Model. Note that you should be creating a constraint using the Model::add_ctr method, rather than a constructor.
 */
class Ctr : public Object {
    friend class Model;
    explicit Ctr(ObjectId&& t_ref) : Object(std::move(t_ref)) {}
protected:
    [[nodiscard]] bool isVar() const override { return true; }
    [[nodiscard]] bool isCtr() const override { return false; }
public:
    Ctr(const Ctr& t_var) = default;
    Ctr(Ctr&& t_var) noexcept = default;

    Ctr& operator=(const Ctr& t_var) = default;
    Ctr& operator=(Ctr&& t_var) noexcept = default;
};

static std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr) {
    return t_os << t_ctr.name();
}

MAKE_HASHABLE(Ctr)

#endif //OPTIMIZE_CONSTRAINT_H
