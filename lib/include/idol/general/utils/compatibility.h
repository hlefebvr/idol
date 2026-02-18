//
// Created by Henri on 18/02/2026.
//

#ifndef IDOL_COMPATIBILITY_H
#define IDOL_COMPATIBILITY_H

#if __cplusplus >= 202002L
#include <functional>
namespace std {
    template <typename Arg, typename Result>
    using unary_function = __unary_function<Arg, Result>;
}
#endif

#endif //IDOL_COMPATIBILITY_H