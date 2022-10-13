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
};

class Callback::Context {
public:
    virtual ~Context() = default;

    [[nodiscard]] virtual Event event() const = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const;

    [[nodiscard]] virtual Solution::Dual dual_solution() const;

    virtual void add_lazy_cut(TempCtr t_ctr) const;

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
