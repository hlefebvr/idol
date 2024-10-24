//
// Created by henri on 03/02/23.
//

#ifndef IDOL_IMPL_ANNOTATION_H
#define IDOL_IMPL_ANNOTATION_H

#include <string>
#include <memory>
#include <any>

namespace idol {
    class Env;

    namespace impl {
        class Annotation;
    }

}

class idol::impl::Annotation {
    struct Id {
        Env& env;
        const unsigned int id;
        const std::string name;
        std::any default_value;
        Id(Env& t_env, unsigned int t_id, std::string&& t_name) : env(t_env), id(t_id), name(std::move(t_name)) {}
    };
    std::shared_ptr<Id> m_id;
protected:
    template<class T, class ...ArgsT> void set_default_value(ArgsT&& ...t_args);
    template<class T> [[nodiscard]] const T& cast_default_value() const { return std::any_cast<const T&>(m_id->default_value); }
public:
    Annotation(::idol::Env& t_env, bool t_is_var_annotation, std::string&& t_name);

    virtual ~Annotation() = default;

    [[nodiscard]] virtual bool is_var_annotation() const = 0;

    [[nodiscard]] virtual bool is_ctr_annotation() const = 0;

    /**
     * Returns the id of the annotation.
     *
     * The id is unique within the environment.
     * @return The id of the annotation
     */
    [[nodiscard]] unsigned int id() const { return m_id->id; }

    /**
     * Returns the given name of the annotation.
     * @return The given name of the annotation
     */
    [[nodiscard]] const std::string& name() const { return m_id->name; }

    /**
     * Returns true if the annotation has a default value, false otherwise.
     * @return true if the annotation has a default value, false otherwise
     */
    [[nodiscard]] bool has_default() const { return m_id->default_value.has_value(); }

    /**
     * Frees the memory kept for the annotation in the environment.
     */
    void free();

    /**
     * Returns the underlying environment of the annotation.
     * @return The underlying environment of the annotation.
     */
    [[nodiscard]] Env& env() const { return m_id->env; }
};

template<class T, class... ArgsT>
void idol::impl::Annotation::set_default_value(ArgsT &&... t_args) {
    m_id->default_value.emplace<T>(std::forward<ArgsT>(t_args)...);
}

#endif //IDOL_IMPL_ANNOTATION_H
