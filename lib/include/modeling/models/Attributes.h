//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTES_H
#define IDOL_ATTRIBUTES_H

#include <typeinfo>
#include <string>

class Attribute {
    [[nodiscard]] virtual const std::type_info& section() const = 0;
    [[nodiscard]] virtual const std::type_info& type() const = 0;
public:
    [[nodiscard]] virtual unsigned int index() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;

    bool operator==(const Attribute& t_rhs) const {
        if (type() != t_rhs.type()) {
            return false;
        }
        return t_rhs.section() == section() && t_rhs.index() == index();
    }

    bool operator!=(const Attribute& t_rhs) const { return !operator==(t_rhs); }

    template<class T> bool is_in_section(const T& t_section) const { return section() == typeid(t_section); }
};

template<class TypeT>
class AttributeWithType : public Attribute {
    [[nodiscard]] const std::type_info& type() const override { return typeid(TypeT); }
};

template<class TypeT, class ...ArgsT>
class AttributeWithTypeAndArguments : public AttributeWithType<TypeT> {};

namespace impl {
    template<class AlgorithmT, class TypeT, unsigned int Index, class ...ArgsT>
    class AttributeWithType : public ::AttributeWithTypeAndArguments<TypeT, ArgsT...> {
        [[nodiscard]] const std::type_info &section() const override { return typeid(AlgorithmT); }
    public:
        [[nodiscard]] unsigned int index() const override { return Index; }
    };
}

namespace impl::Attr {
    template<class AttrT, class TypeT> struct n_attributes;
    template<class AttrT, class TypeT, unsigned int Index> struct reserve_index;
};

#define IDOL_CREATE_ATTRIBUTE_CLASS(t_attr_class) \
namespace impl::Attr::Sections {                \
    struct t_attr_class {};                                                 \
}                                     \
namespace Attr::Sections { \
    static const ::impl::Attr::Sections::t_attr_class t_attr_class {}; \
};

#define IDOL_CREATE_ATTRIBUTE_TYPE(t_attr_class, t_type, t_size) \
namespace impl::Attr { \
    template<> struct n_attributes<::impl::Attr::Sections::t_attr_class, t_type> { \
        static constexpr unsigned int value = t_size;   \
    };           \
}

#define IDOL_CREATE_ATTRIBUTE(t_attr_class, t_type, t_index, t_name, ...) \
namespace impl::Attr {                                               \
    static_assert((t_index) < ::impl::Attr::n_attributes<::impl::Attr::Sections::t_attr_class, t_type>::value); \
    template<> struct reserve_index<::impl::Attr::Sections::t_attr_class, t_type, t_index> {};   \
}                                                                     \
namespace impl::Attr::t_attr_class {                                                 \
    struct t_name : public ::impl::AttributeWithType<Sections::t_attr_class, ::t_type, t_index, __VA_ARGS__> { \
        std::string name() const override { return std::string("Attr::") + #t_attr_class + "::" + #t_name; } \
    }; \
}                                                                                    \
namespace Attr::t_attr_class {                                                       \
    static const impl::Attr::t_attr_class::t_name t_name {};                                                                                     \
};

template<class> class LinExpr;
template<class Key1, class Key2> class Expr;
class Model;
class Ctr;
class Var;
class Constant;
class Row;
class Column;

namespace Solution {
    class Primal;
    class Dual;
}

using Rhs = LinExpr<Ctr>;
using Obj = Expr<Var, Var>;
using IDOL_int = int;
using IDOL_double = double;
using IDOL_Solution_Primal = Solution::Primal;
using IDOL_Solution_Dual = Solution::Dual;

#endif //IDOL_ATTRIBUTES_H
