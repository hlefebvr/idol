//
// Created by henri on 01/02/23.
//
#include "optimizers/Algorithm.h"
#include "modeling/models/Attributes_Model.h"

Algorithm::Algorithm(const Model &t_model) : Optimizer(t_model) {

}

void Algorithm::optimize() {
    m_is_terminated = false;
    hook_before_optimize();
    hook_optimize();
    hook_after_optimize();
}

void Algorithm::set(const Parameter<int>& t_param, int t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        m_int_parameters.set(t_param, t_value);
        return;
    }

    AttributeManagers::Base::set(t_param, t_value);
}

void Algorithm::set(const Parameter<bool>& t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        m_bool_parameters.set(t_param, t_value);
        return;
    }

    AttributeManagers::Base::set(t_param, t_value);
}

void Algorithm::set(const Parameter<double>& t_param, double t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        m_double_parameters.set(t_param, t_value);
        return;
    }

    AttributeManagers::Base::set(t_param, t_value);
}

[[nodiscard]] int Algorithm::get(const Parameter<int>& t_param) const {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        return m_int_parameters.get(t_param);
    }

    return AttributeManagers::Base::get(t_param);
}

[[nodiscard]] bool Algorithm::get(const Parameter<bool>& t_param) const {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        return m_bool_parameters.get(t_param);
    }

    return AttributeManagers::Base::get(t_param);
}

[[nodiscard]] double Algorithm::get(const Parameter<double>& t_param) const {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        return m_double_parameters.get(t_param);
    }

    return AttributeManagers::Base::get(t_param);
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
