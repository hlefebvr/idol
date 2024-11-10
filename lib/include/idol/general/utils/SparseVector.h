//
// Created by henri on 24.10.24.
//

#ifndef IDOL_SPARSEVECTOR_H
#define IDOL_SPARSEVECTOR_H

#include <vector>
#include <algorithm>
#include <optional>
#include "sort.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/general/numericals.h"
#include <idol/general/utils/SparseVectorTSL.h>
#include <idol/mixed-integer/modeling/variables/Var.h>

namespace idol {
    class Var;
    class Ctr;

    namespace impl {

        template<class T>
        struct hash_ {
            auto operator()(const T& t_obj) const { return std::hash<unsigned int>()(t_obj.id()); }
        };

        template<class T>
        struct equal_to_ {
            auto operator()(const T& t_a, const T& t_b) const { return t_a.id() == t_b.id(); }
        };

        template<class T>
        struct less_ {
            auto operator()(const T& t_a, const T& t_b) const { return t_a.id() < t_b.id(); }
        };

    }
}

template<> struct std::hash<idol::Var> : public idol::impl::hash_<idol::Var> {};
template<> struct std::equal_to<idol::Var> : public idol::impl::equal_to_<idol::Var> {};
template<> struct std::less<idol::Var> : public idol::impl::less_<idol::Var> {};

template<> struct std::hash<idol::Ctr> : public idol::impl::hash_<idol::Ctr> {};
template<> struct std::equal_to<idol::Ctr> : public idol::impl::equal_to_<idol::Ctr> {};
template<> struct std::less<idol::Ctr> : public idol::impl::less_<idol::Ctr> {};

namespace idol {
    template<class ...ArgsT> using SparseVector = SparseVectorTSL<ArgsT...>;
}

#endif //IDOL_SPARSEVECTOR_H
