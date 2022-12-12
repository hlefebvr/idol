//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGER_DELEGATE_H
#define IDOL_ATTRIBUTEMANAGER_DELEGATE_H

#include "AttributeManager.h"
#include "../../errors/AttributeBadRequest.h"

namespace AttributeManagers {
    class Delegate;
}

class Object;

class AttributeManagers::Delegate : public AttributeManager {
protected:
    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute) {
        throw AttributeBadRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute) const {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute);
    }

    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute, const Var& t_object)  {
        throw AttributeBadRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute, const Var& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute, t_object);
    }

    virtual AttributeManager& attribute_delegate(const Attribute& t_attribute, const Ctr& t_object)  {
        throw AttributeBadRequest(t_attribute);
    }

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Attribute& t_attribute, const Ctr& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_attribute, t_object);
    }
public:
    void set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, t_value);
    }

    void set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, t_value);
    }

    void set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, const Column &t_value) override {
        set(t_attr, t_var, Column(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, const Constant &t_value) override {
        set(t_attr, t_var, Constant(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_attr, t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, t_value);
    }

    void set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, const Row &t_value) override {
        set(t_attr, t_ctr, Row(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, const Constant &t_value) override {
        set(t_attr, t_ctr, Constant(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<int, void> &t_attr, int t_value) override {
        attribute_delegate(t_attr).set(t_attr, t_value);
    }

    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, const LinExpr<Ctr> &t_value) override {
        set(t_attr, LinExpr<Ctr>(t_value));
    }

    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, const Expr<Var, Var> &t_value) override {
        set(t_attr, Expr<Var, Var>(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, const Constant &t_value) override {
        set(t_attr, Constant(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, Constant &&t_value) override {
        attribute_delegate(t_attr).set(t_attr, std::move(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, const Constant &t_value) override {
        set(t_attr, t_ctr, t_var, Constant(t_value));
    }

    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_attr, t_ctr).set(t_attr, t_ctr, t_var, std::move(t_value));
    }

    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Column &get(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Constant &get(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Row &get(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Constant &get(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const LinExpr<Ctr> &get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Expr<Var, Var> &get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Constant &get(const AttributeWithTypeAndArguments<Constant, void> &t_attr) const override {
        return attribute_delegate(t_attr).get(t_attr);
    }

    [[nodiscard]] const Constant &get(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var) const override {
        return attribute_delegate(t_attr, t_ctr).get(t_attr, t_ctr, t_var);
    }
};

#endif //IDOL_ATTRIBUTEMANAGER_DELEGATE_H
