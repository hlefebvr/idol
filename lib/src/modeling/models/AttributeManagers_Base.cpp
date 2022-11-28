//
// Created by henri on 28/11/22.
//
#include "../../../include/modeling/models/AttributeManagers_Base.h"
#include "../../../include/modeling/matrix/Column.h"
#include "../../../include/modeling/matrix/Row.h"

void AttributeManagers::Base::set(const Attribute<double> &t_attr, const Var& t_var, double t_value) {
    set_attr_var_double(t_attr, t_var, t_value);
}

void AttributeManagers::Base::set(const Attribute<int> &t_attr, const Var &t_var, int t_value) {
    set_attr_var_int(t_attr, t_var, t_value);
}

void AttributeManagers::Base::set(const Attribute<Column> &t_attr, const Var &t_var, const Column &t_value) {
    set_attr_var_Column(t_attr, t_var, Column(t_value));
}

void AttributeManagers::Base::set(const Attribute<Column> &t_attr, const Var &t_var, Column &&t_value) {
    set_attr_var_Column(t_attr, t_var, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Var &t_var, const Constant &t_value) {
    set_attr_var_Constant(t_attr, t_var, Constant(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Var &t_var, Constant &&t_value) {
    set_attr_var_Constant(t_attr, t_var, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<int> &t_attr, const Ctr &t_ctr, int t_value) {
    set_attr_ctr_int(t_attr, t_ctr, t_value);
}

void AttributeManagers::Base::set(const Attribute<Row> &t_attr, const Ctr &t_ctr, const Row &t_value) {
    set_attr_ctr_Row(t_attr, t_ctr, Row(t_value));
}

void AttributeManagers::Base::set(const Attribute<Row> &t_attr, const Ctr &t_ctr, Row &&t_value) {
    set_attr_ctr_Row(t_attr, t_ctr, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Constant &t_value) {
    set_attr_ctr_Constant(t_attr, t_ctr, Constant(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, Constant &&t_value) {
    set_attr_ctr_Constant(t_attr, t_ctr, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<int> &t_attr, int t_value) {
    set_attr_int(t_attr, t_value);
}

void AttributeManagers::Base::set(const Attribute<LinExpr<Ctr>> &t_attr, const LinExpr<Ctr> &t_value) {
    set_attr_LinExpr_Ctr(t_attr, LinExpr<Ctr>(t_value));
}

void AttributeManagers::Base::set(const Attribute<LinExpr<Ctr>> &t_attr, LinExpr<Ctr> &&t_value) {
    set_attr_LinExpr_Ctr(t_attr, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<Expr<Var>> &t_attr, const Expr<Var> &t_value) {
    set_attr_Expr_Var_Var(t_attr, Expr<Var>(t_value));
}

void AttributeManagers::Base::set(const Attribute<Expr<Var>> &t_attr, Expr<Var> &&t_value) {
    set_attr_Expr_Var_Var(t_attr, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Constant &t_value) {
    set_attr_Constant(t_attr, Constant(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, Constant &&t_value) {
    set_attr_Constant(t_attr, std::move(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var, const Constant &t_value) {
    set_attr_ctr_var_Constant(t_attr, t_ctr, t_var, Constant(t_value));
}

void AttributeManagers::Base::set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) {
    set_attr_ctr_var_Constant(t_attr, t_ctr, t_var, std::move(t_value));
}

double AttributeManagers::Base::get(const Attribute<double> &t_attr, const Var &t_var) const {
    return get_attr_var_double(t_attr, t_var);
}

int AttributeManagers::Base::get(const Attribute<int> &t_attr, const Var &t_var) const {
    return get_attr_var_int(t_attr, t_var);
}

const Column &AttributeManagers::Base::get(const Attribute<Column> &t_attr, const Var &t_var) const {
    return get_attr_var_Column(t_attr, t_var);
}

const Constant &AttributeManagers::Base::get(const Attribute<Constant> &t_attr, const Var &t_var) const {
    return get_attr_var_Constant(t_attr, t_var);
}

int AttributeManagers::Base::get(const Attribute<int> &t_attr, const Ctr &t_ctr) const {
    return get_attr_ctr_int(t_attr, t_ctr);
}

const Row &AttributeManagers::Base::get(const Attribute<Row> &t_attr, const Ctr &t_ctr) const {
    return get_attr_ctr_Row(t_attr, t_ctr);
}

const Constant &AttributeManagers::Base::get(const Attribute<Constant> &t_attr, const Ctr &t_ctr) const {
    return get_attr_ctr_Constant(t_attr, t_ctr);
}

int AttributeManagers::Base::get(const Attribute<int> &t_attr) const {
    return get_attr_int(t_attr);
}

const LinExpr<Ctr> &AttributeManagers::Base::get(const Attribute<LinExpr<Ctr>> &t_attr) const {
    return get_attr_LinExpr_Ctr(t_attr);
}

const Expr<Var> &AttributeManagers::Base::get(const Attribute<Expr<Var>> &t_attr) const {
    return get_attr_Expr_Var_Var(t_attr);
}

const Constant &AttributeManagers::Base::get(const Attribute<Constant> &t_attr) const {
    return get_attr_Constant(t_attr);
}

const Constant &AttributeManagers::Base::get(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var) const {
    return get_attr_ctr_var_Constant(t_attr, t_ctr, t_var);
}

