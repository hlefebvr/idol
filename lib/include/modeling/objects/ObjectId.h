//
// Created by henri on 23/11/22.
//

#ifndef IDOL_OBJECTID_H
#define IDOL_OBJECTID_H

#include <string>

class ObjectId {
    static unsigned int s_id;

    const unsigned int m_id;
    const unsigned int m_index;
    const std::string m_name;
public:
    ObjectId(unsigned int t_index, std::string&& t_name, std::string&& t_default_name);

    ObjectId(const ObjectId&) = default;
    ObjectId(ObjectId&&) noexcept = default;

    ObjectId& operator=(const ObjectId&) = delete;
    ObjectId& operator=(ObjectId&&) noexcept = delete;

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    [[nodiscard]] const std::string& name() const { return m_name; }
};

#endif //IDOL_OBJECTID_H
