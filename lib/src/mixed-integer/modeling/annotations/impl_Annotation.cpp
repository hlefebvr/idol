//
// Created by henri on 03/02/23.
//

#include "idol/mixed-integer/modeling/annotations/impl_Annotation.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

idol::impl::Annotation::Annotation(::idol::Env &t_env, std::string &&t_name)
        : m_id([&]() {
            const auto id = t_env.create_annotation_id();
            auto name = t_name.empty() ? "a" + std::to_string(id) : std::move(t_name);
            return std::make_shared<Id>(t_env, id, std::move(name));
        }()) {}

void idol::impl::Annotation::free() {
    m_id->env.free_annotation_id(*this);
}
