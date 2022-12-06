//
// Created by henri on 13/10/22.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "../../modeling/constraints/TempCtr.h"
#include "../../modeling/solutions/Solution.h"
#include "../../errors/NotImplemented.h"

enum Event {
    NewIncumbentFound,
    RelaxationSolved,
    Unsupported
};

class Callback {
public:
    virtual ~Callback() = default;

    class Context;

    virtual void execute(Context& t_ctx) = 0;

    virtual Solution::Primal help() const { throw std::runtime_error("experimental"); }

    virtual void update_var_lb(const Var& t_var, double t_lb) {
        throw NotImplemented("Updating lb from callback", "update_var_lb");
    }

    virtual void update_var_ub(const Var& t_var, double t_lb) {
        throw NotImplemented("Updating ub from callback", "update_var_ub");
    }

    [[nodiscard]] virtual double get_lb(const Var& t_var) const {
        throw NotImplemented("Getting lb from callback", "get_lb");
    }

    [[nodiscard]] virtual double get_ub(const Var& t_var) const {
        throw NotImplemented("Getting ub from callback", "get_ub");
    }

};

class Callback::Context {
public:
    virtual ~Context() = default;

    [[nodiscard]] virtual Event event() const = 0;

    [[nodiscard]] virtual Solution::Primal node_primal_solution() const;

    [[nodiscard]] virtual Solution::Dual node_dual_solution() const;

    virtual Ctr add_lazy_cut(TempCtr t_ctr);

    virtual bool submit_solution(Solution::Primal&& t_solution);

};

namespace Callbacks {
    namespace impl {
        template<class T> struct type_holder { using type = T; };
    }

    // uses_lazy_cuts
    namespace impl {
        template<class T> constexpr decltype(T::uses_lazy_cuts, bool{}) uses_lazy_cuts(const type_holder<T>&) { return T::uses_lazy_cuts; }
        template<class T> constexpr bool uses_lazy_cuts(const T&) { return false; }
    }
    template<class T> constexpr bool uses_lazy_cuts() { return impl::uses_lazy_cuts(impl::type_holder<T>{}); }

    // uses_user_cuts
    namespace impl {
        template<class T> constexpr decltype(T::uses_user_cuts, bool{}) uses_user_cuts(const type_holder<T>&) { return T::uses_user_cuts; }
        template<class T> constexpr bool uses_user_cuts(const T&) { return false; }
    }
    template<class T> constexpr bool uses_user_cuts() { return impl::uses_user_cuts(impl::type_holder<T>{}); }

    // uses_advanced_constructor
    namespace impl {
        template<class T> constexpr decltype(T::uses_advanced_constructor, bool{}) uses_advanced_constructor(const type_holder<T>&) { return T::uses_advanced_constructor; }
        template<class T> constexpr bool uses_advanced_constructor(const T&) { return false; }
    }
    template<class T> constexpr bool uses_advanced_constructor() { return impl::uses_advanced_constructor(impl::type_holder<T>{}); }
}


static std::ostream& operator<<(std::ostream& t_os, Event t_event) {
    switch (t_event) {
        case NewIncumbentFound: return t_os << "NewIncumbentFound";
        case Unsupported: return t_os << "Unsupported";
        default:;
    }
    throw Exception("Enum out of range.");
}

#endif //IDOL_CALLBACK_H
