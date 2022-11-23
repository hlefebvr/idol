//
// Created by henri on 23/11/22.
//

#ifndef IDOL_OBJECTREF_H
#define IDOL_OBJECTREF_H

#include <string>

class ObjectRef {
    static unsigned int s_id;

    const unsigned int m_id;
    const unsigned int m_index;
    const std::string m_name;
public:
    ObjectRef(unsigned int t_index, std::string&& t_name, std::string&& t_default_name);

    ObjectRef(const ObjectRef&) = default;
    ObjectRef(ObjectRef&&) noexcept = default;

    ObjectRef& operator=(const ObjectRef&) = delete;
    ObjectRef& operator=(ObjectRef&&) noexcept = delete;

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    [[nodiscard]] const std::string& name() const { return m_name; }
};

#endif //IDOL_OBJECTREF_H
