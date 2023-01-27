//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYOBJECTVERSIONS_H
#define IDOL_MYOBJECTVERSIONS_H

#include <vector>
#include <optional>
#include "../../errors/Exception.h"
#include "MyModel.h"

class MyModel;

/**
 * This class is used to store the different versions associated to a given object (e.g., a variable).
 * @tparam T the class representing an object's version
 */
template<class T>
class MyVersions {
    static const unsigned int s_buffer_size = 10;

    std::vector<std::optional<T>> m_versions; /// Every versions are stored here, the index corresponds to the id of the model to which the version is associated
public:
    template<class ...ArgsT> void create(const MyModel& t_model, unsigned int t_index, ArgsT&& ...t_args);

    void remove(const MyModel& t_model);

    [[nodiscard]] bool has(const MyModel& t_model) const;

    [[nodiscard]] const T& get(const MyModel& t_model) const;

    [[nodiscard]] T& get(const MyModel& t_model);
};

template<class T>
template<class ...ArgsT>
void MyVersions<T>::create(const MyModel &t_model, unsigned int t_index, ArgsT&& ...t_args) {
    const unsigned int index = t_model.id();

    if (m_versions.size() <= index) {
        m_versions.resize(index + s_buffer_size);
    } else if (m_versions[index].has_value()) {
        throw Exception("Trying to add twice a given object to model.");
    }

    m_versions[index] = T(t_index, std::forward<ArgsT>(t_args)...);
}

template<class T>
const T &MyVersions<T>::get(const MyModel &t_model) const {
    return m_versions[t_model.id()].value();
}

template<class T>
T &MyVersions<T>::get(const MyModel &t_model) {
    return m_versions[t_model.id()].value();
}

template<class T>
void MyVersions<T>::remove(const MyModel &t_model) {
    if (!has(t_model)) { return; }
    m_versions[t_model.id()].reset();
}

template<class T>
bool MyVersions<T>::has(const MyModel &t_model) const {
    const unsigned int index = t_model.id();
    return index < m_versions.size() && m_versions[index].has_value();
}

#endif //IDOL_MYOBJECTVERSIONS_H
