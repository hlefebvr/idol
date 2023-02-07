//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGER_DELEGATE_H
#define IDOL_ATTRIBUTEMANAGER_DELEGATE_H

#include "AttributeManager.h"
#include "../../errors/UnsupportedRequest.h"
#include "modeling/matrix/Column.h"
#include "modeling/matrix/Row.h"

namespace AttributeManagers {
    class Delegate;
}

class AttributeManagers::Delegate : public AttributeManager {
protected:
    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute) {
        throw UnsupportedRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute) const {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute);
    }

    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute, const Var& t_object)  {
        throw UnsupportedRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute, const Var& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute, t_object);
    }

    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute, const Ctr& t_object)  {
        throw UnsupportedRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute, const Ctr& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute, t_object);
    }

    virtual AttributeManager& parameter_delegate(const Parameter<double>& t_param) {
        throw UnsupportedParameter(t_param);
    }

    virtual AttributeManager& parameter_delegate(const Parameter<int>& t_param) {
        throw UnsupportedParameter(t_param);
    }

    virtual AttributeManager& parameter_delegate(const Parameter<bool>& t_param) {
        throw UnsupportedParameter(t_param);
    }

    virtual const AttributeManager& parameter_delegate(const Parameter<double>& t_param) const {
        return const_cast<Delegate*>(this)->parameter_delegate(t_param);
    }

    virtual const AttributeManager& parameter_delegate(const Parameter<int>& t_param) const {
        return const_cast<Delegate*>(this)->parameter_delegate(t_param);
    }

    virtual const AttributeManager& parameter_delegate(const Parameter<bool>& t_param) const {
        return const_cast<Delegate*>(this)->parameter_delegate(t_param);
    }
public:
    void set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, t_value);
    }

    void set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, t_value);
    }

    void set(const Req<Column, Var> &t_attr, const Var &t_var, const Column &t_value) override {
        set(t_attr, t_var, Column(t_value));
    }

    void set(const Req<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const Req<Constant, Var> &t_attr, const Var &t_var, const Constant &t_value) override {
        set(t_attr, t_var, Constant(t_value));
    }

    void set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, t_value);
    }

    void set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, const Row &t_value) override {
        set(t_attr, t_ctr, Row(t_value));
    }

    void set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, const Constant &t_value) override {
        set(t_attr, t_ctr, Constant(t_value));
    }

    void set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const Req<int, void> &t_attr, int t_value) override {
        attribute_delegate(t_attr).set(t_attr, t_value);
    }

    void set(const Req<double, Ctr> &t_attr, const Ctr &t_ctr, double t_value) override {
        attribute_delegate(t_attr).set(t_attr, t_ctr, t_value);
    }

    void set(const Req<LinExpr<Ctr>, void> &t_attr, const LinExpr<Ctr> &t_value) override {
        set(t_attr, LinExpr<Ctr>(t_value));
    }

    void set(const Req<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const Req<Expr<Var, Var>, void> &t_attr, const Expr<Var, Var> &t_value) override {
        set(t_attr, Expr<Var, Var>(t_value));
    }

    void set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const Req<Constant, void> &t_attr, const Constant &t_value) override {
        set(t_attr, Constant(t_value));
    }

    void set(const Req<Constant, void> &t_attr, Constant &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, const Constant &t_value) override {
        set(t_attr, t_ctr, t_var, Constant(t_value));
    }

    void set(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, t_var, std::move(t_value));
    }

    [[nodiscard]] double get(const Req<double, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const Req<int, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Column &get(const Req<Column, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Constant &get(const Req<Constant, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const Req<int, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] double get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Row &get(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Constant &get(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] int get(const Req<int, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] double get(const Req<double, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const LinExpr<Ctr> &get(const Req<LinExpr<Ctr>, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Expr<Var, Var> &get(const Req<Expr<Var, Var>, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Constant &get(const Req<Constant, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Constant &get(const Req<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr, t_var);
    }

    void set(const Parameter<double> &t_param, double t_value) override {
        parameter_delegate(t_param).set(t_param, t_value);
    }

    void set(const Parameter<bool> &t_param, bool t_value) override {
        parameter_delegate(t_param).set(t_param, t_value);
    }

    void set(const Parameter<int> &t_param, int t_value) override {
        parameter_delegate(t_param).set(t_param, t_value);
    }

    double get(const Parameter<double> &t_param) const override {
        return parameter_delegate(t_param).get(t_param);
    }

    bool get(const Parameter<bool> &t_param) const override {
        return parameter_delegate(t_param).get(t_param);
    }

    int get(const Parameter<int> &t_param) const override {
        return parameter_delegate(t_param).get(t_param);
    }
};

#endif //IDOL_ATTRIBUTEMANAGER_DELEGATE_H
