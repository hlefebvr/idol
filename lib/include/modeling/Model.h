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

template class Model<Decision>;
template class Model<Parameter>;

#endif //OPTIMIZE_MODEL_H
