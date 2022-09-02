//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include <vector>
#include "Env.h"
#include "Constraint.h"
#include "../containers/SafePointer.h"
#include "../containers/Optional.h"

template<enum Player PlayerT = Decision>
class Model {
    Env& m_env;
    std::vector<SafePointer<impl::Variable>> m_variables;
    std::vector<SafePointer<impl::Variable>> m_parameters;
    std::vector<SafePointer<impl::Constraint<PlayerT>>> m_constraints;

    template<class T, class ...Args> T* add_one(std::vector<SafePointer<T>>& t_destination, Args... t_args);
    template<enum Player GenPlayerT> std::vector<SafePointer<impl::Variable>>& generic_variable_container();
    template<enum Player GenPlayerT> void update(const Variable<GenPlayerT>& t_variable, const Optional<double>& t_lower_bound, const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type);
public:
    explicit Model(Env& t_env);

    Model(const Model&) = delete;
    Model(Model&&) noexcept = delete;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    Variable<PlayerT> add_variable(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name = "");
    Variable<opp_player_v<PlayerT>> add_parameter(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name = "");
    Constraint<PlayerT> add_constraint(TempConstraint<PlayerT> t_temp_ctr, std::string t_name = "");

    void update(const Variable<PlayerT>& t_variable, const Optional<double>& t_lower_bound, const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type);
    void update(const Variable<opp_player_v<PlayerT>>& t_parameter, const Optional<double>& t_lower_bound, const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type);
};

template<enum Player PlayerT>
Model<PlayerT>::Model(Env &t_env) : m_env(t_env) {

}

template<enum Player PlayerT>
template<class T, class... Args>
T* Model<PlayerT>::add_one(std::vector<SafePointer<T>> &t_destination, Args... t_args) {
    const unsigned int index = t_destination.size();
    auto* ptr_to_impl = ((ObjectCreator&) m_env).create<T>(index, std::forward<Args>(t_args)...);
    t_destination.emplace_back(ptr_to_impl);
    return ptr_to_impl;
}

template<enum Player PlayerT>
Variable<PlayerT> Model<PlayerT>::add_variable(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name) {
    return Variable<PlayerT>(
            add_one<impl::Variable>(m_variables, t_lower_bound, t_upper_bound, t_type, std::move(t_name))
    );
}

template<enum Player PlayerT>
Variable<opp_player_v<PlayerT>>
Model<PlayerT>::add_parameter(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name) {
    return Variable<opp_player_v<PlayerT>>(
            add_one<impl::Variable>(m_parameters, t_lower_bound, t_upper_bound, t_type, std::move(t_name))
    );
}

template<enum Player PlayerT>
Constraint<PlayerT> Model<PlayerT>::add_constraint(TempConstraint<PlayerT> t_temp_ctr, std::string t_name) {
    return Constraint<PlayerT>(
            add_one<impl::Constraint<PlayerT>>(m_constraints, std::move(t_temp_ctr), std::move(t_name))
    );
}

template<enum Player PlayerT>
template<enum Player GenPlayerT>
void Model<PlayerT>::update(const Variable<GenPlayerT> &t_variable, const Optional<double> &t_lower_bound,
                            const Optional<double> &t_upper_bound, const Optional<VariableType> &t_type) {
    auto& generic_variable = *generic_variable_container<GenPlayerT>()[t_variable.index()];

    if (t_lower_bound.has_value()) {
        generic_variable.set_lower_bound(t_lower_bound.value());
    }

    if (t_upper_bound.has_value()) {
        generic_variable.set_upper_bound(t_upper_bound.value());
    }

    if (t_type.has_value()) {
        generic_variable.set_type(t_type.value());
    }
}

template<enum Player PlayerT>
template<enum Player GenPlayerT>
std::vector<SafePointer<impl::Variable>> &Model<PlayerT>::generic_variable_container() {
    if constexpr (GenPlayerT == PlayerT) {
        return m_variables;
    } else {
        return m_parameters;
    }
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Variable<PlayerT>& t_variable, const Optional<double>& t_lower_bound,
                            const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type) {
    update<PlayerT>(t_variable, t_lower_bound, t_upper_bound, t_type);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Variable<opp_player_v<PlayerT>> &t_parameter, const Optional<double> &t_lower_bound,
                            const Optional<double> &t_upper_bound, const Optional<VariableType> &t_type) {
    update<opp_player_v<PlayerT>>(t_parameter, t_lower_bound, t_upper_bound, t_type);
}

#endif //OPTIMIZE_MODEL_H
