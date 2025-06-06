//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VERSIONS_H
#define IDOL_VERSIONS_H

#include <vector>
#include <optional>
#include <any>
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/general/utils/Optional.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol {
    class Model;

    template<class T>
    class Versions;
}

/**
 * This class is used to store the different versions associated to a given object (e.g., a variable).
 * @tparam T the class representing an object's version
 */
template<class T>
class idol::Versions {
    static const unsigned int s_buffer_size = 10;

    std::vector<Optional<T>> m_versions; /// Every versions are stored here, the index corresponds to the id of the model to which the version is associated
    std::vector<std::any> m_annotations;
public:
    template<class ...ArgsT> explicit Versions(ArgsT&& ...t_args) : m_versions({ idol::make_optional<T>(std::forward<ArgsT&&>(t_args)...) }) {}

    Versions(const Versions&) = delete;
    Versions(Versions&&) noexcept = delete;

    Versions& operator=(const Versions&) = delete;
    Versions& operator=(Versions&&) noexcept = delete;

    template<class ...ArgsT> void create(const Model& t_model, unsigned int t_index, ArgsT&& ...t_args);

    void remove(const Model& t_model);

    [[nodiscard]] bool has(const Model& t_model) const;

    [[nodiscard]] const T& get(const Model& t_model) const;

    T& get(const Model& t_model);

    [[nodiscard]] const T& get_default() const;

    T& get_default();

    template<class ValueT> const ValueT* get_annotation(unsigned int t_index) const;

    template<class ValueT, class ...ArgsT> void set_annotation(unsigned int t_index, ArgsT&& ...t_args) {
        if (t_index >= m_annotations.size()) {
            m_annotations.resize(t_index + s_buffer_size);
        }
        m_annotations[t_index] = ValueT(std::forward<ArgsT>(t_args)...);
    }
};

template<class T>
template<class ValueT>
const ValueT* idol::Versions<T>::get_annotation(unsigned int t_index) const {
    if (t_index >= m_annotations.size() || !m_annotations[t_index].has_value()) {
        return nullptr;
    }
    return &std::any_cast<const ValueT&>(m_annotations[t_index]);
}

template<class T>
const T &idol::Versions<T>::get_default() const {
    return *m_versions.front();
}

template<class T>
T &idol::Versions<T>::get_default() {
    return *m_versions.front();
}

template<class T>
template<class ...ArgsT>
void idol::Versions<T>::create(const Model &t_model, unsigned int t_index, ArgsT&& ...t_args) {

    const unsigned int index = t_model.id();

    if (m_versions.size() <= index) {
        m_versions.resize(index + s_buffer_size);
    } else if (m_versions[index].has_value()) {
        throw Exception("Object already in model.");
    }

    m_versions[index] = T(t_index, std::forward<ArgsT>(t_args)...);
}

template<class T>
const T &idol::Versions<T>::get(const Model &t_model) const {
    const unsigned int id = t_model.id();
    if (id >= m_versions.size() || !m_versions[id].has_value()) {
        throw Exception("Object not part of model.");
    }
    return m_versions[id].value();
}

template<class T>
T &idol::Versions<T>::get(const Model &t_model) {
    const unsigned int id = t_model.id();
    if (id >= m_versions.size() || !m_versions[id].has_value()) {
        throw Exception("Object not part of model.");
    }
    return m_versions[id].value();
}

template<class T>
void idol::Versions<T>::remove(const Model &t_model) {
    if (!has(t_model)) { return; }
    m_versions[t_model.id()].reset();
}

template<class T>
bool idol::Versions<T>::has(const Model &t_model) const {
    const unsigned int index = t_model.id();
    return index < m_versions.size() && m_versions[index].has_value();
}

#endif //IDOL_VERSIONS_H
