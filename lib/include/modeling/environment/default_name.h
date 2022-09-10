//
// Created by henri on 10/09/22.
//

#ifndef OPTIMIZE_DEFAULT_NAME_H
#define OPTIMIZE_DEFAULT_NAME_H

#include <string>

template<class T>
struct default_name {
    static constexpr const char* value = "Object";
};

#define SET_DEFAULT_OBJECT_NAME(T) \
class T;                                  \
template<> \
struct default_name<T> {\
    static constexpr const char* value = #T;                               \
};

SET_DEFAULT_OBJECT_NAME(Var);
SET_DEFAULT_OBJECT_NAME(Ctr);

template<class T> static const char* default_name_v = default_name<T>::value;

#endif //OPTIMIZE_DEFAULT_NAME_H
