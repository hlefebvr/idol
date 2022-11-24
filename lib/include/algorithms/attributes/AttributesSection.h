//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTESSECTION_H
#define OPTIMIZE_ATTRIBUTESSECTION_H

#include "../logs/Log.h"
#include <array>
#include <any>
#include <optional>
#include <functional>

template<unsigned int N_PARAMETERS>
class AttributesSection {
    std::array<std::optional<std::any>, N_PARAMETERS> m_values {};
    std::array<std::function<void(const std::any&)>, N_PARAMETERS> m_callbacks;
public:
    virtual ~AttributesSection() = default;

    template<class T>
    void set(typename T::value_type t_value) {
        typename T::schema().check(t_value);
        idol_Log(Trace, "attributes", "Setting value of " << T::name() << " to " << t_value << ".");
        if (m_callbacks[T::index]) {
            m_callbacks[T::index](t_value);
        }
        m_values[T::index] = std::move(t_value);
    }

    template<class T>
    void set_callback(std::function<void(const typename T::value_type&)> t_cb) {
        m_callbacks[T::index] = [cb = std::move(t_cb)](const std::any& t_any){
            cb(std::any_cast<typename T::value_type>(t_any));
        };
    }

    template<class T>
    [[nodiscard]] typename T::value_type get() const {
        if (auto optional = m_values[T::index]; optional.has_value()) {
            return std::any_cast<typename T::value_type>(optional.value());
        }
        return T::default_value();
    }

    static constexpr unsigned int size = N_PARAMETERS;
};

#endif //OPTIMIZE_ATTRIBUTESSECTION_H
