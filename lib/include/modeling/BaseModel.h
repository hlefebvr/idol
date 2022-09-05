//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_BASEMODEL_H
#define OPTIMIZE_BASEMODEL_H

#include <vector>
#include "Env.h"
#include "Constraint.h"
#include "../containers/SafePointer.h"
#include "../containers/Optional.h"

template<enum Player PlayerT = Decision>
class BaseModel {

    Env& m_env;

    std::vector<Variable<PlayerT>> m_variables;
    std::vector<Variable<opp_player_v<PlayerT>>> m_parameters;
    std::vector<Constraint<PlayerT>> m_constraints;

    template<class T> std::vector<T>& container();
    template<class T> void free(std::vector<T>& t_vec);
protected:
    explicit BaseModel(Env& t_env);

    template<class T, class ...Args> T add_impl(Args... t_args);
    template<class T> typename T::impl_t& impl(const T& t_obj);
public:
    virtual ~BaseModel();

    BaseModel(const BaseModel&) = delete;
    BaseModel(BaseModel&&) noexcept = delete;

    BaseModel& operator=(const BaseModel&) = delete;
    BaseModel& operator=(BaseModel&&) noexcept = delete;
};

template<enum Player PlayerT>
BaseModel<PlayerT>::BaseModel(Env& t_env) : m_env(t_env) {

}

template<enum Player PlayerT>
template<class T>
std::vector<T> &BaseModel<PlayerT>::container() {
    if constexpr (std::is_same_v<Variable<PlayerT>, T>) {
        return m_variables;
    } else if constexpr (std::is_same_v<Variable<opp_player_v<PlayerT>>, T>) {
        return m_parameters;
    } else if constexpr (std::is_same_v<Constraint<PlayerT>, T>) {
        return m_constraints;
    }
    throw std::runtime_error("Unexpected object type required");
}

template<enum Player PlayerT>
template<class T, class... Args>
T BaseModel<PlayerT>::add_impl(Args... t_args) {
    auto& vec = container<T>();
    const unsigned int index = vec.size();
    auto* ptr_to_impl = ((ObjectCreator&) m_env).create<typename T::impl_t>(index, std::forward<Args>(t_args)...);
    vec.emplace_back(ptr_to_impl);
    return vec.back();
}

template<enum Player PlayerT>
template<class T>
typename T::impl_t& BaseModel<PlayerT>::impl(const T& t_obj) {
    auto& vec = container<T>();
    const unsigned int index = t_obj.index();
    if (index >= vec.size() || vec[index].id() != t_obj.id()) {
        throw std::runtime_error("The desired constraint is not part of the model.");
    }
    return ((ExtractableObject<typename T::impl_t>&) vec[index]).impl();
}

template<enum Player PlayerT>
template<class T>
void BaseModel<PlayerT>::free(std::vector<T> &t_vec) {
    for (auto& var : t_vec) {
        impl(var).free();
    }
}

template<enum Player PlayerT>
BaseModel<PlayerT>::~BaseModel() {

    free(m_variables);
    free(m_parameters);
    free(m_constraints);

}

#endif //OPTIMIZE_BASEMODEL_H
