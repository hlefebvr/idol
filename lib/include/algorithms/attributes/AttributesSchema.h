//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTESSCHEMA_H
#define OPTIMIZE_ATTRIBUTESSCHEMA_H

#include "../../errors/Exception.h"
#include <string>

template<class ...RequireT>
class AttributesSchema {

    template<class ...T> class Checker;

    template<class T>
    struct Checker<T> {
        static void check(const T&) {}
    };

    template<class T, class First, class ...Others>
    struct Checker<T, First, Others...> {
        static void check(const T& t_value) {
            First().throw_if_not_valid(t_value);
            Checker<T, Others...>::check(t_value);
        }
    };
public:
    template<class T>
    void check(const T& t_value) {
        Checker<T, RequireT...>().check(t_value);
    }

};

namespace AttributesSchemaProperty {

    template<long int Value>
    struct GreaterThan {
        template<class T>
        void throw_if_not_valid(const T &t_value) {
            if (t_value <= Value) {
                throw Exception("Value should be (stricly) greater than " + std::to_string(Value) + ".");
            }
        }
    };

    template<long int Value>
    struct GreaterOrEqualTo {
        template<class T>
        void throw_if_not_valid(const T &t_value) {
            if (t_value < Value) {
                throw Exception("Value should be (stricly) greater than " + std::to_string(Value) + ".");
            }
        }
    };

    template<long int Value>
    struct LessThan {
        template<class T>
        void throw_if_not_valid(const T &t_value) {
            if (t_value >= Value) {
                throw Exception("Value should be (stricly) less than " + std::to_string(Value) + ".");
            }
        }
    };

    template<long int L, long int R>
    struct InRange {
        template<class T>
        void throw_if_not_valid(const T &t_value) {
            if (((T) L) > t_value || t_value > ((T) R)) {
                throw Exception("Value should be between " + std::to_string(L) + " and " + std::to_string(R) + ".");
            }
        }
    };

}

#endif //OPTIMIZE_ATTRIBUTESSCHEMA_H
