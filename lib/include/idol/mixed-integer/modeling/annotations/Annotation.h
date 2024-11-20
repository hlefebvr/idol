//
// Created by henri on 03/02/23.
//

#ifndef IDOL_ANNOTATION_H
#define IDOL_ANNOTATION_H

#include "impl_Annotation.h"

namespace idol {
    template<class ValueT>
    class Annotation;
}

template<class ValueT = unsigned int>
class idol::Annotation : public impl::Annotation {
public:
    Annotation(Env& t_env, std::string t_name) : impl::Annotation(t_env, std::move(t_name)) {}

    template<class ...ArgsT> Annotation(Env& t_env, std::string t_name, ArgsT&& ...t_args)
        : impl::Annotation(t_env, std::move(t_name)) {

        set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);

    }

    template<class ...ArgsT>
    static Annotation<ValueT> make_with_default_value(Env& t_env, std::string t_name, ArgsT&& ...t_args) {
        Annotation<ValueT> result(t_env, std::move(t_name));
        result.set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);
        return result;
    }

    [[nodiscard]] const ValueT& default_value() const { return impl::Annotation::cast_default_value<ValueT>(); }
};

#endif //IDOL_ANNOTATION_H
