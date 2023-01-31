//
// Created by henri on 31/01/23.
//

#ifndef IDOL_BACKEND_H
#define IDOL_BACKEND_H

#include <string>

class Model;
class Var;
class Ctr;
class Constant;

namespace impl {
    class Backend;
}

class impl::Backend {
    const ::Model& m_parent;
protected:
    virtual void initialize() = 0;

    virtual void add(const Var& t_var) = 0;
    virtual void add(const Ctr& t_ctr) = 0;

    virtual void remove(const Var& t_var) = 0;
    virtual void remove(const Ctr& t_ctr) = 0;

    virtual void update(const Var& t_var) = 0;
    virtual void update(const Ctr& t_ctr) = 0;

    virtual void update_objective() = 0;
    virtual void update_objective_sense() = 0;
    virtual void update_matrix(const Ctr& t_ctr, const Var& t_var, const Constant& t_constant) = 0;
    virtual void update_rhs() = 0;

    virtual void optimize() = 0;

    virtual void update() = 0;

    virtual void write(const std::string& t_name) = 0;

    static double as_numeric(const Constant& t_constant);
public:
    explicit Backend(const ::Model& t_parent) : m_parent(t_parent) {}

    virtual ~Backend() = default;

    [[nodiscard]] const ::Model& parent() const { return m_parent; }
};

class Backend : public impl::Backend {
    friend class Model;
public:
    explicit Backend(const Model& t_parent) : impl::Backend(t_parent) {}
};

#endif //IDOL_BACKEND_H
