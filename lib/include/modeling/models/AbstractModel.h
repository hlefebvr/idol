//
// Created by henri on 07/02/23.
//

#ifndef IDOL_ABSTRACTMODEL_H
#define IDOL_ABSTRACTMODEL_H

#include "backends/Backend.h"

#include "modeling/attributes/AttributeManager_Delegate.h"
#include "Attributes_Model.h"
#include "../constraints/Attributes_Ctr.h"
#include "../variables/Attributes_Var.h"
#include "backends/parameters/Timer.h"
#include "backends/parameters/Parameters_Algorithm.h"

class Backend;

class AbstractModel : public AttributeManagers::Delegate {
    friend class Idol;

    Timer m_timer;
    std::unique_ptr<Backend> m_backend;
protected:
    // Backend
    virtual void set_backend(Backend* t_backend) { m_backend.reset(t_backend); m_backend->initialize(); }
    virtual void throw_if_no_backend() const { if (!m_backend) { throw Exception("No backend was found."); } }
    Backend& backend() { throw_if_no_backend(); return *m_backend; }
    [[nodiscard]] const Backend& backend() const { throw_if_no_backend(); return *m_backend; }
    void reset_backend() { m_backend.reset(); }
    bool has_backend() { return (bool) m_backend; }
public:
    // Variables
    virtual void add(const Var& t_var) = 0;
    [[nodiscard]] virtual bool has(const Var& t_var) const = 0;
    virtual void remove(const Var& t_var) = 0;
    [[nodiscard]] virtual ConstIteratorForward<std::vector<Var>> vars() const = 0;

    // Constraints
    virtual void add(const Ctr& t_ctr) = 0;
    [[nodiscard]] virtual bool has(const Ctr& t_ctr) const = 0;
    virtual void remove(const Ctr& t_ctr) = 0;
    [[nodiscard]] virtual ConstIteratorForward<std::vector<Ctr>> ctrs() const = 0;

    template<class T, unsigned int N> void add_array(const Vector<T, N>& t_vector);
    template<class T, class ...ArgsT> void add_many(const T& t_object, const ArgsT& ...t_args);

    // Model
    [[nodiscard]] virtual unsigned int id() const = 0;
    [[nodiscard]] virtual Env& env() const = 0;
    [[nodiscard]] virtual AbstractModel* clone() const = 0;

    // Backend
    void optimize() {
        throw_if_no_backend();
        m_timer.start();
        backend().optimize();
        m_timer.stop();
    }
    [[nodiscard]] const Timer& time() const { return m_timer; }
    [[nodiscard]] double remaining_time() const { return std::max(0., get(Param::Algorithm::TimeLimit) - time().count()); }

    using AttributeManagers::Delegate::get;
    using AttributeManagers::Delegate::set;
};

template<class T, class... ArgsT>
void AbstractModel::add_many(const T &t_object, const ArgsT &... t_args) {
    add(t_object);
    if constexpr (sizeof...(t_args) > 0) {
        add_many(t_args...);
    }
}

template<class T, unsigned int N>
void AbstractModel::add_array(const Vector<T, N> &t_vector) {
    if constexpr (N == 1) {
        for (const auto& x : t_vector) {
            add(x);
        }
    } else  {
        for (const auto& x : t_vector) {
            add_array<T, N - 1>(x);
        }
    }
}

template<class ObjectT>
auto save(const AbstractModel& t_model, const Req<double, ObjectT>& t_attr) {

    std::conditional_t<std::is_same_v<ObjectT, Var>, Solution::Primal, Solution::Dual> result;

    const int sense = t_model.get(Attr::Obj::Sense);
    const int status = t_model.get(Attr::Solution::Status);
    const int reason = t_model.get(Attr::Solution::Reason);

    result.set_status(status);
    result.set_reason(reason);

    if (status == Infeasible) {
        result.set_objective_value(sense == Minimize ? Inf : -Inf);
        return result;
    }

    if (status == Unbounded) {
        result.set_objective_value(sense == Minimize ? -Inf : Inf);
        return result;
    }

    result.set_objective_value(t_model.get(Attr::Solution::ObjVal));

    if constexpr (std::is_same_v<ObjectT, Var>) {
        for (const auto &var: t_model.vars()) {
            result.set(var, t_model.get(t_attr, var));
        }
    } else {
        for (const auto &ctr: t_model.ctrs()) {
            result.set(ctr, t_model.get(t_attr, ctr));
        }
    }

    return result;
}

struct Idol {

    template<class T, class ModelT, class ...ArgsT>
    static T &set_optimizer(ModelT &t_model, ArgsT &&...t_args) {
        auto *result = new T(t_model, std::forward<ArgsT>(t_args)...);
        ((AbstractModel&) t_model).set_backend(result);
        return *result;
    }

};

static std::ostream& operator<<(std::ostream& t_os, const AbstractModel& t_model) {

    if (t_model.get(Attr::Obj::Sense) == Minimize) {
        t_os << "Minimize";
    } else {
        t_os << "Maximize";
    }

    t_os << " " << t_model.get(Attr::Obj::Expr) << "\nSubject to:\n";
    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get(Attr::Ctr::Row, ctr);
        const auto& linear = row.linear();
        const auto& quadratic = row.quadratic();
        const auto type = t_model.get(Attr::Ctr::Type, ctr);

        t_os << ctr << ": ";

        if (linear.empty()) {
            t_os << quadratic;
        } else {
            t_os << linear;
            if (!quadratic.empty()) {
                t_os << " + " << quadratic;
            }
        }

        switch (type) {
            case LessOrEqual: t_os << " <= "; break;
            case Equal: t_os << " = "; break;
            case GreaterOrEqual: t_os << " >= "; break;
            default: t_os << " ?undefined? ";
        }

        t_os << row.rhs() << '\n';
    }

    t_os << "Variables:\n";
    for (const auto& var : t_model.vars()) {

        const double lb = t_model.get(Attr::Var::Lb, var);
        const double ub = t_model.get(Attr::Var::Ub, var);
        const int type = t_model.get(Attr::Var::Type, var);

        if (!is_neg_inf(lb) && !is_pos_inf(ub)) {
            t_os << lb << " <= " << var << " <= " << ub;
        } else if (!is_pos_inf(ub)) {
            t_os << var << " <= " << ub;
        } else if (!is_neg_inf(lb)) {
            t_os << var << " >= " << lb;
        } else {
            t_os << var;
        }

        if (type == Binary) {
            t_os << " [binary]";
        } else if (type == Integer) {
            t_os << " [integer]";
        }

        t_os << '\n';
    }
    return t_os;
}

#endif //IDOL_ABSTRACTMODEL_H
