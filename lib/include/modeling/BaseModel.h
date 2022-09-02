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
    std::vector<SafePointer<impl::Variable>> m_variables;
    std::vector<SafePointer<impl::Variable>> m_parameters;
    std::vector<SafePointer<impl::Constraint<PlayerT>>> m_constraints;

    template<class T> std::vector<SafePointer<typename T::impl_t>>& container();
    template<class T> T& get_impl_or_throw(std::vector<SafePointer<T>>& t_container, const AbstractObject& t_obj);
protected:
    explicit BaseModel(Env& t_env);

    template<class T, class ...Args> T add_impl(Args... t_args);

    impl::Constraint<PlayerT>& impl(const Constraint<PlayerT> &t_ctr);
    impl::Variable& impl(const Variable<PlayerT> &t_var);
    impl::Variable& impl(const Variable<opp_player_v<PlayerT>> &t_param);
public:
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
std::vector<SafePointer<typename T::impl_t>> &BaseModel<PlayerT>::container() {
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
template<class T>
T& BaseModel<PlayerT>::get_impl_or_throw(std::vector<SafePointer<T>>& t_container, const AbstractObject& t_obj) {
    const unsigned int index = t_obj.index();
    if (index >= t_container.size() || t_container[index]->id() != t_obj.id()) {
        throw std::runtime_error("The desired constraint is not part of the model.");
    }
    return *m_constraints[index];
}

template<enum Player PlayerT>
impl::Constraint<PlayerT> &BaseModel<PlayerT>::impl(const Constraint<PlayerT> &t_ctr) {
    return get_impl_or_throw(m_constraints, t_ctr);
}

template<enum Player PlayerT>
impl::Variable &BaseModel<PlayerT>::impl(const Variable<opp_player_v<PlayerT>> &t_param) {
    return get_impl_or_throw(m_parameters, t_param);
}

template<enum Player PlayerT>
impl::Variable &BaseModel<PlayerT>::impl(const Variable<PlayerT> &t_var) {
    return get_impl_or_throw(m_variables, t_var);
}

template<enum Player PlayerT>
template<class T, class... Args>
T BaseModel<PlayerT>::add_impl(Args... t_args) {
    auto& vec = container<T>();
    const unsigned int index = vec.size();
    auto* ptr_to_impl = ((ObjectCreator&) m_env).create<typename T::impl_t>(index, std::forward<Args>(t_args)...);
    vec.emplace_back(ptr_to_impl);
    return T(ptr_to_impl);
}

#endif //OPTIMIZE_BASEMODEL_H
