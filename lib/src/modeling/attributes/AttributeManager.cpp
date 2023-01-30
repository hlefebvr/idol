//
// Created by henri on 06/12/22.
//
#include "modeling/attributes/AttributeManager.h"
#include "modeling/matrix/Column.h"
#include "modeling/matrix/Row.h"

void AttributeManager::set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, const Column &t_value) {
    set(t_attr, t_var, Column(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, const Constant &t_value) {
    set(t_attr, t_var, Constant(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, const Row &t_value) {
    set(t_attr, t_ctr, Row(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, const Constant &t_value) {
    set(t_attr, t_ctr, Constant(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, const LinExpr<Ctr> &t_value) {
    set(t_attr, LinExpr<Ctr>(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, const Expr<Var, Var> &t_value) {
    set(t_attr, Expr<Var, Var>(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, const Constant &t_value) {
    set(t_attr, Constant(t_value));
}

void AttributeManager::set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, const Constant &t_value) {
    set(t_attr, t_ctr, t_var, Constant(t_value));
}

