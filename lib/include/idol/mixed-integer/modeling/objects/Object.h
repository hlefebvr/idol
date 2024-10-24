//
// Created by henri on 27/01/23.
//

#ifndef IDOL_OBJECT_H
#define IDOL_OBJECT_H

#include <memory>
#include "ObjectId.h"
#include "idol/general/utils/Vector.h"
#include "idol/general/utils/Pair.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/general/utils/exceptions/Exception.h"

namespace idol {
    class Model;

    template<class T, class CRTP>
    class Object;
}

template<class T, class CRTP>
class idol::Object {
    std::shared_ptr<ObjectId<T>> m_object_id;
protected:
    [[nodiscard]] auto& versions() { return m_object_id->versions(); }

    [[nodiscard]] const auto& versions() const { return m_object_id->versions(); }

    template<class ...ArgsT>
    void create_version(const Model& t_model, unsigned int t_index, ArgsT&& ...t_args) const {
        m_object_id->versions().create(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    void remove_version(const Model& t_model) const {
        m_object_id->versions().remove(t_model);
    }
public:
    explicit Object(ObjectId<T>&& t_object_id) : m_object_id(std::make_shared<ObjectId<T>>(std::move(t_object_id))) {}

    Object(const Object&) = default;
    Object(Object&&) noexcept = default;

    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) noexcept = default;

    /**
     * Returns the name of the optimization object.
     * @return The name of the optimization object.
     */
    [[nodiscard]] const std::string& name() const { return m_object_id->name(); }

    /**
     * Returns the id of the optimization object.
     * @return The id of the optimization object.
     */
    [[nodiscard]] unsigned int id() const { return m_object_id->id(); }

    /**
     * Returns true if the optimization object is part of the model `t_model`, false otherwise.
     * @param t_model The model.
     * @return True if the optimization object is part of the model `t_model`, false otherwise.
     */
    [[nodiscard]] bool is_in(const Model& t_model) const { return m_object_id->versions().has(t_model); }

    /**
     * Returns the value of the given annotation `t_annotation` associated to the object.
     *
     * If no value is found, the default value of the annotation is returned. If no default value was set, an exception is
     * thrown.
     * @tparam ValueT The value type of the annotation.
     * @param t_annotation The annotation.
     * @return The value of the annotation.
     */
    template<class ValueT> const ValueT& get(const Annotation<CRTP, ValueT>& t_annotation) const {
        const auto& result = m_object_id->versions().template get_annotation<ValueT>(t_annotation.id());
        if (result) {
            return *result;
        }
        if (t_annotation.has_default()) {
            return t_annotation.default_value();
        }
        throw Exception("No value could be found and no default value was given for annotation " + t_annotation.name());
    }

    /**
     * Sets the value of the given annotation `t_annotation` associated to the object.
     * @tparam ValueT The value type of the annotation.
     * @tparam ArgsT The parameter pack types for constructing the value of the annotation.
     * @param t_annotation The annotation.
     * @param t_args The parameter pack arguments used to construct "in place" the value of the annotation.
     */
    template<class ValueT, class ...ArgsT> void set(const Annotation<CRTP, ValueT>& t_annotation, ArgsT&& ...t_args) const { m_object_id->versions().template set_annotation<ValueT, ArgsT...>(t_annotation.id(), std::forward<ArgsT>(t_args)...); }
};

namespace idol {

    template<class T, class CRTP>
    static std::ostream &operator<<(std::ostream &t_os, const Object<T, CRTP> &t_var) {
        return t_os << t_var.name();
    }

}

namespace idol::impl {

    template<class U, unsigned int N, unsigned int I = 0>
    static ::idol::Vector<U, N - I> create_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<U(const std::string& t_name)>& t_add_one) {
        ::idol::Vector<U, N - I> result;
        const unsigned int size = t_dims[I];
        result.reserve(size);
        for (unsigned int i = 0 ; i < size ; ++i) {
            const std::string name = t_name + "_" + std::to_string(i);
            if constexpr (I == N - 1) {
                result.emplace_back( t_add_one(name) );
            } else {
                result.emplace_back( create_many<U, N, I+1>(t_dims, name, t_add_one) );
            }
        }
        return result;
    }

}

#define IDOL_MAKE_HASHABLE(name) \
template<> \
struct std::hash<idol::name> { \
    std::size_t operator()(const idol::name& t_variable) const { \
        return std::hash<unsigned int>()(t_variable.id()); \
    } \
};                          \
\
template<> \
struct std::equal_to<idol::name> { \
    std::size_t operator()(const idol::name& t_a, const idol::name& t_b) const { \
        return t_a.id() == t_b.id(); \
    } \
}; \
template<> \
struct std::less<idol::name> { \
    std::size_t operator()(const idol::name& t_a, const idol::name& t_b) const { \
        return t_a.id() < t_b.id(); \
    } \
};                               \
template<> \
struct std::equal_to<idol::Pair<idol::name, idol::name>> { \
    std::size_t operator()(const idol::Pair<idol::name, idol::name>& t_a, const idol::Pair<idol::name, idol::name>& t_b) const { \
        return std::equal_to<idol::name>()(t_a.first, t_b.first) && std::equal_to<idol::name>()(t_a.second, t_b.second); \
    } \
};

#endif //IDOL_OBJECT_H
