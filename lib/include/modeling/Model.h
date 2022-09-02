//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include "BaseModel.h"

template<enum Player PlayerT = Decision>
class Model : public BaseModel<PlayerT> {
    template<enum Player GenPlayerT>
    void update(const Variable<GenPlayerT>& t_variable, const Optional<double>& t_lower_bound, const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type);

    using BaseModel<PlayerT>::impl;
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
    void update(const Constraint<PlayerT>& t_ctr, const Variable<PlayerT>& t_variable, double t_coeff);
    void update(const Constraint<PlayerT>& t_ctr, const Variable<PlayerT>& t_variable, LinExpr<opp_player_v<PlayerT>> t_coeff);
    void update(const Constraint<PlayerT>& t_ctr, const Variable<PlayerT>& t_variable, const Variable<opp_player_v<PlayerT>>&, double t_coeff);
    void update(const Constraint<PlayerT>& t_ctr, const Variable<opp_player_v<PlayerT>>&, double t_coeff);
    void update(const Constraint<PlayerT>& t_ctr, LinExpr<opp_player_v<PlayerT>> t_constant);
    void update(const Constraint<PlayerT>& t_ctr, double t_rhs);
};

template<enum Player PlayerT>
Model<PlayerT>::Model(Env &t_env) : BaseModel<PlayerT>(t_env) {

}

template<enum Player PlayerT>
Variable<PlayerT> Model<PlayerT>::add_variable(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name) {
    return this->template add_impl<Variable<PlayerT>>(t_lower_bound, t_upper_bound, t_type, std::move(t_name));
}

template<enum Player PlayerT>
Variable<opp_player_v<PlayerT>>
Model<PlayerT>::add_parameter(double t_lower_bound, double t_upper_bound, VariableType t_type, std::string t_name) {
    return this->template add_impl<Variable<opp_player_v<PlayerT>>>(t_lower_bound, t_upper_bound, t_type, std::move(t_name));
}

template<enum Player PlayerT>
Constraint<PlayerT> Model<PlayerT>::add_constraint(TempConstraint<PlayerT> t_temp_ctr, std::string t_name) {
    return this->template add_impl<Constraint<PlayerT>>(std::move(t_temp_ctr), std::move(t_name));
}

template<enum Player PlayerT>
template<enum Player GenPlayerT>
void Model<PlayerT>::update(const Variable<GenPlayerT> &t_variable, const Optional<double> &t_lower_bound,
                            const Optional<double> &t_upper_bound, const Optional<VariableType> &t_type) {
    auto& generic_variable = impl(t_variable);

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
void Model<PlayerT>::update(const Variable<PlayerT>& t_variable, const Optional<double>& t_lower_bound,
                            const Optional<double>& t_upper_bound, const Optional<VariableType>& t_type) {
    update<PlayerT>(t_variable, t_lower_bound, t_upper_bound, t_type);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Variable<opp_player_v<PlayerT>> &t_parameter, const Optional<double> &t_lower_bound,
                            const Optional<double> &t_upper_bound, const Optional<VariableType> &t_type) {
    update<opp_player_v<PlayerT>>(t_parameter, t_lower_bound, t_upper_bound, t_type);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, const Variable<PlayerT> &t_variable, double t_coeff) {
    impl(t_ctr).expr().set_coefficient(t_variable, t_coeff);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, const Variable<PlayerT> &t_variable,
                            LinExpr<opp_player_v<PlayerT>> t_coeff) {
    impl(t_ctr).expr().set_exact_coefficient(t_variable, std::move(t_coeff));
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, const Variable<PlayerT> &t_variable,
                            const Variable<opp_player_v<PlayerT>> &t_param, double t_coeff) {
    impl(t_ctr).expr().set_coefficient(t_variable, t_param, t_coeff);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, const Variable<opp_player_v<PlayerT>> &t_param, double t_coeff) {
    impl(t_ctr).expr().set_coefficient(t_param, t_coeff);
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, LinExpr<opp_player_v<PlayerT>> t_constant) {
    this->template impl(t_ctr).expr().set_exact_constant(std::move(t_constant));
}

template<enum Player PlayerT>
void Model<PlayerT>::update(const Constraint<PlayerT> &t_ctr, double t_rhs) {
    this->template impl(t_ctr).expr().set_numerical_constant(t_rhs);
}

#endif //OPTIMIZE_MODEL_H
