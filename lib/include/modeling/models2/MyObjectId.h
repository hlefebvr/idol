//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYOBJECTID_H
#define IDOL_MYOBJECTID_H

#include <list>
#include <string>

template<class T> class MyVersions;

template<class T>
class MyObjectId {
    const typename std::list<MyVersions<T>>::iterator m_it;
    unsigned int m_id;
    const std::string m_name;
public:
    MyObjectId(typename std::list<MyVersions<T>>::iterator t_it, unsigned int t_id, std::string t_name)
    : m_it(t_it), m_id(t_id), m_name(std::move(t_name)) {}

    auto& versions() { return *m_it; }
    [[nodiscard]] const auto& versions() const { return *m_it; }

    [[nodiscard]] const auto& name() const { return m_name; }
};

#endif //IDOL_MYOBJECTID_H
