//
// Created by henri on 11/01/23.
//

#ifndef IDOL_ENV_H
#define IDOL_ENV_H

#include "../objects/ObjectStore.h"
#include "../objects/ObjectAttributesWithUserAttr.h"
#include "../variables/Var.h"
#include "../constraints/Ctr.h"
#include "errors/AttributeBadRequest.h"

template <class T>
struct explicit_template_param
{
    using type = T;
};

template <class T>
using explicit_template_param_t = typename explicit_template_param<T>::type;

class Env {
    static std::unique_ptr<Env> s_env;
    ObjectStore<Var, ObjectAttributesWithUserAttr<Var>> m_variables;
    ObjectStore<Ctr, ObjectAttributesWithUserAttr<Ctr>> m_constraints;
    ObjectStore<UserAttr, ObjectAttributes<UserAttr>> m_user_attributes;
public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;

    Var add_var(std::string&& t_name);

    Ctr add_ctr(std::string&& t_name);

    template<class T> UserAttr add_user_attr(const explicit_template_param_t<T>& t_default_value, std::string t_name = "");

    template<class T> [[nodiscard]] const T& get(const UserAttr& t_annotation, const Var& t_var) const;

    static Env& global();
};


template<class T>
UserAttr Env::add_user_attr(const explicit_template_param_t <T> &t_default_value, std::string t_name) {
    auto ref = m_user_attributes.add_attributes(std::move(t_name), "Annotation");
    UserAttr user_attribute(std::move(ref), t_default_value);
    m_user_attributes.add_object(user_attribute);

    return user_attribute;
}

template<class T>
const T &Env::get(const UserAttr &t_annotation, const Var &t_var) const {
    if (t_annotation.type() != typeid(T)) {
        throw AttributeBadRequest(t_annotation);
    }
    return m_variables.attributes(t_var).get_user_attribute<T>(t_annotation);
}

#endif //IDOL_ENV_H
