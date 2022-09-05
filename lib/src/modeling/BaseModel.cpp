//
// Created by henri on 05/09/22.
//
#include "modeling/BaseModel.h"

template<enum Player PlayerT>
BaseModel<PlayerT>::BaseModel(Env& t_env) : m_env(t_env) {

}

template<enum Player PlayerT>
BaseModel<PlayerT>::~BaseModel() {

    free(m_variables);
    free(m_parameters);
    free(m_constraints);

}
