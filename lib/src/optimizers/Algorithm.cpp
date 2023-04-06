//
// Created by henri on 01/02/23.
//
#include "optimizers/Algorithm.h"
#include "modeling/models/Attributes_Model.h"

Algorithm::Algorithm(const Model &t_model) : Optimizer(t_model) {

}

int Algorithm::get_status() const {
    return m_status;
}

int Algorithm::get_reason() const {
    return m_reason;
}

double Algorithm::get_best_obj() const {
    return m_best_obj;
}

double Algorithm::get_best_bound() const {
    return m_best_bound;
}

double Algorithm::get_relative_gap() const {
    return relative_gap(m_best_bound, m_best_obj);
}

double Algorithm::get_absolute_gap() const {
    return absolute_gap(m_best_bound, m_best_obj);
}
