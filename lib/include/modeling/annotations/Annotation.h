//
// Created by henri on 03/02/23.
//

#ifndef IDOL_ANNOTATION_H
#define IDOL_ANNOTATION_H

#include "impl_Annotation.h"

class Var;
class Ctr;

template<class ObjectT, class ValueT = unsigned int>
class Annotation : public impl::Annotation {
    static_assert(std::is_same_v<ObjectT, Var> || std::is_same_v<ObjectT, Ctr>);
public:
    Annotation(Env& t_env, std::string t_name) : impl::Annotation(t_env, std::is_same_v<ObjectT, Var>, std::move(t_name)) {}

    template<class ...ArgsT> Annotation(Env& t_env, std::string t_name, ArgsT&& ...t_args)
        : impl::Annotation(t_env, std::is_same_v<ObjectT, Var>, std::move(t_name)) {

        set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);

    }

    template<class ...ArgsT>
    static Annotation<ObjectT, ValueT> make_with_default_value(Env& t_env, std::string t_name, ArgsT&& ...t_args) {
        Annotation<ObjectT, ValueT> result(t_env, std::move(t_name));
        result.set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);
        return result;
    }

    [[nodiscard]] bool is_var_annotation() const override { return std::is_same_v<ObjectT, Var>; }

    [[nodiscard]] bool is_ctr_annotation() const override { return std::is_same_v<ObjectT, Ctr>; }

    [[nodiscard]] const ValueT& default_value() const { return impl::Annotation::cast_default_value<ValueT>(); }
};

#endif //IDOL_ANNOTATION_H
