//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTE_TOOLS_H
#define OPTIMIZE_ATTRIBUTE_TOOLS_H

#include "AttributeStore.h"
#include "AttributeSchema.h"

namespace AttrType {
    template<class TypeT, unsigned int INDEX>
    struct attribute_index_exists;
}

#define CREATE_ATTRIBUTE_TYPE(t_name, t_size) \
namespace AttrType {                         \
    class t_name : public AttributeStore<t_size> {}; \
}

#define CREATE_ATTRIBUTE(t_attr_type, t_index, t_value_type, t_name, t_default_value, ...) \
static_assert(t_index < AttrType::t_attr_type::size);                                                 \
template<> struct AttrType::attribute_index_exists<AttrType::t_attr_type, t_index> {};                               \
namespace Attr { struct t_name; }                                                                                                 \
struct Attr::t_name { \
    static constexpr unsigned int index = t_index; \
    using attr_type = AttrType::t_attr_type; \
    using value_type = t_value_type;                                                                   \
    using schema = AttributeSchema<__VA_ARGS__>;                                                                           \
    static double default_value() { return t_default_value; }                              \
    static std::string name() { return std::string(#t_attr_type) + "::" + #t_name; }\
};

#endif //OPTIMIZE_ATTRIBUTE_TOOLS_H
