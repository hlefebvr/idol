//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_VARIABLE_H
#define OPTIMIZE_VARIABLE_H

#include "../objects/Object.h"
#include "../Types.h"
#include "../objects/ObjectId.h"
#include <iostream>
#include <memory>

class Column;
class Constant;
class Ctr;
class Model;

/**
 * Decision variable object.
 *
 * A variable always belongs to a single Model. Note that you should be creating a variable using the Model::add_var method, rather than a constructor.
 */
class Var : public Object {
    friend class Model;
    explicit Var(ObjectId&& t_ref) : Object(std::move(t_ref)) {}
protected:
    [[nodiscard]] bool isVar() const override { return true; }
    [[nodiscard]] bool isCtr() const override { return false; }
public:
    Var(const Var& t_var) = default;
    Var(Var&& t_var) noexcept = default;

    Var& operator=(const Var& t_var) = default;
    Var& operator=(Var&& t_var) noexcept = default;
};

static std::ostream& operator<<(std::ostream& t_os, const Var& t_var) {
    return t_os << t_var.name();
}

MAKE_HASHABLE(Var)

#endif //OPTIMIZE_VARIABLE_H
