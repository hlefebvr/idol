//
// Created by henri on 20/12/22.
//

#ifndef IDOL_EVENTS_H
#define IDOL_EVENTS_H

#include <typeinfo>
#include <string>

class EventType {
    [[nodiscard]] virtual const std::type_info& section() const = 0;
public:
    virtual ~EventType() = default;

    [[nodiscard]] virtual unsigned int index() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;

    bool operator==(const EventType& t_rhs) const {
        return t_rhs.section() == section() && t_rhs.index() == index();
    }

    bool operator!=(const EventType& t_rhs) const { return !operator==(t_rhs); }

    template<class T> bool is_in_section(const T& t_section) const { return section() == typeid(t_section); }
};

#define IDOL_CREATE_EVENT_CLASS(t_name, t_size) \
namespace Event_::Sections {                           \
    struct t_name { static constexpr unsigned int size = t_size; }; \
}                                               \
namespace impl::Event_ {              \
    template<class SectionT, unsigned int Index> class reserve_index; \
}

#define IDOL_CREATE_EVENT(t_section, t_name, t_index) \
static_assert(t_index < Event_::Sections::t_section::size); \
template<> class impl::Event_::reserve_index<::Event_::Sections::t_section, t_index> {};                                                      \
namespace impl::Event_::t_section {                                     \
    class t_name : public ::EventType {               \
        const std::type_info& section() const override { return typeid(t_name); }  \
    public:                                           \
        unsigned int index() const override { return t_index; }\
        std::string name() const override { return #t_name; }                                              \
    };                                                      \
} \
namespace Event_::t_section {                                     \
    static const ::impl::Event_::t_section::t_name t_name; \
}

#endif //IDOL_EVENTS_H
