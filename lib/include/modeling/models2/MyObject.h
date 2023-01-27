//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYOBJECT_H
#define IDOL_MYOBJECT_H

#include <memory>
#include "MyObjectId.h"

class MyModel;

template<class T>
class MyObject {
    std::shared_ptr<MyObjectId<T>> m_object_id;
protected:
    [[nodiscard]] auto& versions() { return m_object_id->versions(); }

    [[nodiscard]] const auto& versions() const { return m_object_id->versions(); }

    template<class ...ArgsT>
    void create_version(const MyModel& t_model, unsigned int t_index, ArgsT&& ...t_args) const {
        m_object_id->versions().create(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    void remove_version(const MyModel& t_model) const {
        m_object_id->versions().remove(t_model);
    }
public:
    MyObject(typename std::list<MyVersions<T>>::iterator t_it, unsigned int t_id, std::string t_name)
    : m_object_id(std::make_shared<MyObjectId<T>>(t_it, t_id, std::move(t_name))) {}

    MyObject(const MyObject&) = default;
    MyObject(MyObject&&) noexcept = default;

    MyObject& operator=(const MyObject&) = default;
    MyObject& operator=(MyObject&&) noexcept = default;

    [[nodiscard]] const std::string& name() const { return m_object_id->name(); }
};

#endif //IDOL_MYOBJECT_H
