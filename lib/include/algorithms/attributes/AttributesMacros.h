//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTESMACROS_H
#define OPTIMIZE_ATTRIBUTESMACROS_H

#include "AttributesSection.h"
#include "AttributesSchema.h"

namespace AttributesSections {
    template<class TypeT, unsigned int INDEX>
    struct attribute_index_exists;
}

#define CREATE_ATTRIBUTE_SECTION(t_name, t_size) \
namespace AttributesSections {                         \
    class t_name : public AttributesSection<t_size> {}; \
}

#define CREATE_ATTRIBUTE(t_attr_type, t_index, t_value_type, t_name, t_default_value, ...) \
static_assert(t_index < AttributesSections::t_attr_type::size);                                                 \
template<> struct AttributesSections::attribute_index_exists<AttributesSections::t_attr_type, t_index> {};                               \
namespace Attr { struct t_name; }                                                                                                 \
struct Attr::t_name { \
    static constexpr unsigned int index = t_index; \
    using attr_type = AttributesSections::t_attr_type; \
    using value_type = t_value_type;                                                                   \
    using schema = AttributesSchema<__VA_ARGS__>;                                                                           \
    static double default_value() { return t_default_value; }                              \
    static std::string name() { return std::string(#t_attr_type) + "::" + #t_name; }\
};

#endif //OPTIMIZE_ATTRIBUTESMACROS_H
