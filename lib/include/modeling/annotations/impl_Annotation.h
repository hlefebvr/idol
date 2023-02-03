//
// Created by henri on 03/02/23.
//

#ifndef IDOL_IMPL_ANNOTATION_H
#define IDOL_IMPL_ANNOTATION_H

#include <string>
#include <memory>

class Env;

namespace impl {
    class Annotation;
}

class impl::Annotation {
    struct Id {
        Env& env;
        const unsigned int id;
        const std::string name;
        Id(Env& t_env, unsigned int t_id, std::string&& t_name) : env(t_env), id(t_id), name(std::move(t_name)) {}
    };
    std::shared_ptr<Id> m_id;
public:
    Annotation(::Env& t_env, bool t_is_var_annotation, std::string&& t_name);

    virtual ~Annotation() = default;

    [[nodiscard]] virtual bool is_var_annotation() const = 0;

    [[nodiscard]] virtual bool is_ctr_annotation() const = 0;

    [[nodiscard]] unsigned int id() const { return m_id->id; }

    [[nodiscard]] const std::string& name() const { return m_id->name; }

    void free();
};

#endif //IDOL_IMPL_ANNOTATION_H
