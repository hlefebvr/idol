//
// Created by henri on 25/11/22.
//

#ifndef IDOL_PARAMETERS_H
#define IDOL_PARAMETERS_H

#include <array>
#include <string>
#include <optional>

template<class TypeT>
class Parameter {
    static_assert(std::is_trivial_v<TypeT>);
    [[nodiscard]] virtual const std::type_info& underlying_type() const = 0;
public:
    virtual ~Parameter() = default;
    [[nodiscard]] virtual unsigned int index() const = 0;
    [[nodiscard]] virtual TypeT default_value() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;

    template<class T> bool operator==(const Parameter<T>& t_param) const {
        if constexpr (!std::is_same_v<T, TypeT>) {
            return false;
        } else {
            return t_param.underlying_type() == underlying_type() && t_param.index() == index();
        }
    }

    template<class T> bool operator!=(const Parameter<T>& t_param) const { return !operator==(t_param); }

    template<class T> bool is_in_section(const T& t_section) const { return underlying_type() == typeid(t_section); }
};

namespace impl {
    template<class AlgorithmT, class TypeT, unsigned int Index>
    class Parameter : public ::Parameter<TypeT> {
        [[nodiscard]] const std::type_info &underlying_type() const override { return typeid(AlgorithmT); }
    public:
        [[nodiscard]] unsigned int index() const override { return Index; }
    };
}

namespace impl::Param {
    template<class TypeT, unsigned int Size>
    class values {
        std::array<std::optional<TypeT>, Size> m_values;
    public:
        void set(const ::Parameter<TypeT>& t_param, TypeT t_value) { m_values.at(t_param.index()) = t_value; }
        [[nodiscard]] TypeT get(const ::Parameter<TypeT>& t_param) const {
            if (const auto optional = m_values.at(t_param.index()) ; optional.has_value()) {
                return optional.value();
            }
            return t_param.default_value();
        }
    };
}

#define IDOL_CREATE_PARAMETER_CLASS(t_algorithm) \
namespace impl::Param::Sections {                \
    struct t_algorithm {};                                                 \
}                                     \
namespace Param::Sections { \
    static const ::impl::Param::Sections::t_algorithm t_algorithm {}; \
}; \

#define IDOL_CREATE_PARAMETER_TYPE(t_algorithm, t_type, t_size) \
namespace impl::Param { \
    static constexpr unsigned int n_parameters_for_##t_algorithm##_of_type_##t_type = t_size; \
} \
namespace Param::t_algorithm { \
    template<class> struct values; \
    template<> struct values<t_type> : public :: impl::Param::values<t_type, t_size> {}; \
};

#define IDOL_CREATE_PARAMETER(t_algorithm, t_type, t_index, t_name, t_default_value) \
static_assert((t_index) < impl::Param::n_parameters_for_##t_algorithm##_of_type_##t_type);\
namespace impl::Param::t_algorithm {                                                 \
    struct reserve_index_##t_type##_##t_index {};                                                                                 \
    struct t_name : public ::impl::Parameter<Sections::t_algorithm, t_type, t_index> { \
        t_type default_value() const override { return t_default_value; } \
        std::string name() const override { return std::string("Param::") + #t_algorithm + "::" + #t_name; } \
    }; \
}                                                                                    \
namespace Param::t_algorithm {                                                       \
    static const impl::Param::t_algorithm::t_name t_name {};                                                                                     \
};


#endif //IDOL_PARAMETERS_H
