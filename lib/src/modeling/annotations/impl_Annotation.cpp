//
// Created by henri on 03/02/23.
//

#include "idol/modeling//annotations/impl_Annotation.h"
#include "idol/modeling//objects/Env.h"

idol::impl::Annotation::Annotation(::idol::Env &t_env, bool t_is_var_annotation, std::string &&t_name)
        : m_id([&]() {
            auto id = t_env.create_annotation_id(t_is_var_annotation);
            auto name =t_name.empty() ? "Annotation_" + std::to_string(t_is_var_annotation) + "_" + std::to_string(t_is_var_annotation) : std::move(t_name);
            return std::make_shared<Id>(t_env, id, std::move(name));
        }()) {}

void idol::impl::Annotation::free() {
    m_id->env.free_annotation_id(*this);
}
