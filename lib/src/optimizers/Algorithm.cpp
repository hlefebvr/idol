//
// Created by henri on 01/02/23.
//
#include "optimizers/Algorithm.h"
#include "modeling/models/Attributes_Model.h"

Algorithm::Algorithm(const Model &t_model) : Optimizer(t_model) {

}

double Algorithm::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return m_best_obj;
    }

    if (t_attr == Attr::Solution::RelGap) {
        return relative_gap(m_best_bound, m_best_obj);
    }

    if (t_attr == Attr::Solution::AbsGap) {
        return absolute_gap(m_best_bound, m_best_obj);
    }

    if (t_attr == Attr::Solution::BestBound) {
        return m_best_bound;
    }

    if (t_attr == Attr::Solution::BestObj) {
        return m_best_obj;
    }

    return Base::get(t_attr);
}

int Algorithm::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Solution::Status) {
        return m_status;
    }

    if (t_attr == Attr::Solution::Reason) {
        return m_reason;
    }

    return Base::get(t_attr);
}
