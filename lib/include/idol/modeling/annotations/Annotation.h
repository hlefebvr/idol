//
// Created by henri on 03/02/23.
//

#ifndef IDOL_ANNOTATION_H
#define IDOL_ANNOTATION_H

#include "impl_Annotation.h"

namespace idol {
    class Var;
    class Ctr;

    template<class ObjectT, class ValueT>
    class Annotation;
}

template<class ObjectT, class ValueT = unsigned int>
class idol::Annotation : public impl::Annotation {
    static_assert(std::is_same_v<ObjectT, Var> || std::is_same_v<ObjectT, Ctr>);
public:
    /**
     * Constructor.
     *
     * Creates a new annotation stored in `t_env` with name `t_name`. No default value is given.
     * @param t_env The environment which will store the annotation
     * @param t_name The given name to the annotation
     */
    Annotation(Env& t_env, std::string t_name) : impl::Annotation(t_env, std::is_same_v<ObjectT, Var>, std::move(t_name)) {}

    /**
     * Constructor.
     *
     * Creates a new annotation stored in `t_env` with name `t_name` and a default value. The default value is built
     * "in place" by calling the constructor of `ValueT` with the arguments given after `t_name`.
     * @tparam ArgsT Parameter pack template types for constructing the default value of the annotation
     * @param t_env The environment which will store the annotation
     * @param t_name The given name to the annotation
     * @param t_args Parameter pack arguments for constructing the default value of the annotation
     */
    template<class ...ArgsT> Annotation(Env& t_env, std::string t_name, ArgsT&& ...t_args)
        : impl::Annotation(t_env, std::is_same_v<ObjectT, Var>, std::move(t_name)) {

        set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);

    }

    /**
     * Creates a new annotation stored in `t_env` with name `t_name` and a default value. The default value is built
     * "in place" by calling the constructor of `ValueT` with the arguments given after `t_name`.
     * @tparam ArgsT Parameter pack template types for constructing the default value of the annotation
     * @param t_env The environment which will store the annotation
     * @param t_name The given name to the annotation
     * @param t_args Parameter pack arguments for constructing the default value of the annotation
     * @return The created annotation
     */
    template<class ...ArgsT>
    static Annotation<ObjectT, ValueT> make_with_default_value(Env& t_env, std::string t_name, ArgsT&& ...t_args) {
        Annotation<ObjectT, ValueT> result(t_env, std::move(t_name));
        result.set_default_value<ValueT>(std::forward<ArgsT>(t_args)...);
        return result;
    }

    /**
     * Returns true if the annotation is for variables, false otherwise.
     * @return true if the annotation is for variables, false otherwise
     */
    [[nodiscard]] bool is_var_annotation() const override { return std::is_same_v<ObjectT, Var>; }

    /**
     * Returns true if the annotation is for constraints, false otherwise.
     * @return true if the annotation is for constraints, false otherwise
     */
    [[nodiscard]] bool is_ctr_annotation() const override { return std::is_same_v<ObjectT, Ctr>; }

    /**
     * Returns the default value of the annotation.
     * @return The default value of the annotation
     */
    [[nodiscard]] const ValueT& default_value() const { return impl::Annotation::cast_default_value<ValueT>(); }
};

#endif //IDOL_ANNOTATION_H
