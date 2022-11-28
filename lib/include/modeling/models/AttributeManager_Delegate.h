//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGER_DELEGATE_H
#define IDOL_ATTRIBUTEMANAGER_DELEGATE_H

#include "AttributeManager.h"

namespace AttributeManagers {
    class Delegate;
}

class Object;

class AttributeManagers::Delegate : public AttributeManager {
protected:
    virtual AttributeManager& attribute_delegate() = 0;

    [[nodiscard]] virtual const AttributeManager& attribute_delegate() const {
        return const_cast<Delegate*>(this)->attribute_delegate();
    }

    virtual AttributeManager& attribute_delegate(const Var& t_object) = 0;

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Var& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_object);
    }

    virtual AttributeManager& attribute_delegate(const Ctr& t_object) = 0;

    [[nodiscard]] virtual const AttributeManager& attribute_delegate(const Ctr& t_object) const  {
        return const_cast<Delegate*>(this)->attribute_delegate(t_object);
    }
public:
    void set(const Attribute<double> &t_attr, const Var &t_var, double t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, t_value);
    }

    void set(const Attribute<int> &t_attr, const Var &t_var, int t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, t_value);
    }

    void set(const Attribute<Column> &t_attr, const Var &t_var, const Column &t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, t_value);
    }

    void set(const Attribute<Column> &t_attr, const Var &t_var, Column &&t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const Attribute<Constant> &t_attr, const Var &t_var, const Constant &t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, t_value);
    }

    void set(const Attribute<Constant> &t_attr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_var).set(t_attr, t_var, std::move(t_value));
    }

    void set(const Attribute<int> &t_attr, const Ctr &t_ctr, int t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, t_value);
    }

    void set(const Attribute<Row> &t_attr, const Ctr &t_ctr, const Row &t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, t_value);
    }

    void set(const Attribute<Row> &t_attr, const Ctr &t_ctr, Row &&t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Constant &t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, t_value);
    }

    void set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, Constant &&t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, std::move(t_value));
    }

    void set(const Attribute<int> &t_attr, int t_value) override {
        attribute_delegate().set(t_attr, t_value);
    }

    void set(const Attribute<LinExpr<Ctr>> &t_attr, const LinExpr<Ctr> &t_value) override {
        attribute_delegate().set(t_attr, t_value);
    }

    void set(const Attribute<LinExpr<Ctr>> &t_attr, LinExpr<Ctr> &&t_value) override {
        attribute_delegate().set(t_attr, std::move(t_value));
    }

    void set(const Attribute<Expr<Var, Var>> &t_attr, const Expr<Var, Var> &t_value) override {
        attribute_delegate().set(t_attr, t_value);
    }

    void set(const Attribute<Expr<Var, Var>> &t_attr, Expr<Var, Var> &&t_value) override {
        attribute_delegate().set(t_attr, std::move(t_value));
    }

    void set(const Attribute<Constant> &t_attr, const Constant &t_value) override {
        attribute_delegate().set(t_attr, t_value);
    }

    void set(const Attribute<Constant> &t_attr, Constant &&t_value) override {
        attribute_delegate().set(t_attr, std::move(t_value));
    }

    void set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var, const Constant &t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, t_var, t_value);
    }

    void set(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) override {
        attribute_delegate(t_ctr).set(t_attr, t_ctr, t_var, std::move(t_value));
    }

    [[nodiscard]] double get(const Attribute<double> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const Attribute<int> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Column &get(const Attribute<Column> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_var).get(t_attr, t_var);
    }

    [[nodiscard]] const Constant &get(const Attribute<Constant> &t_attr, const Var &t_var) const override {
        return attribute_delegate(t_var).get(t_attr, t_var);
    }

    [[nodiscard]] int get(const Attribute<int> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Row &get(const Attribute<Row> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] const Constant &get(const Attribute<Constant> &t_attr, const Ctr &t_ctr) const override {
        return attribute_delegate(t_ctr).get(t_attr, t_ctr);
    }

    [[nodiscard]] int get(const Attribute<int> &t_attr) const override {
        return attribute_delegate().get(t_attr);
    }

    [[nodiscard]] const LinExpr<Ctr> &get(const Attribute<LinExpr<Ctr>> &t_attr) const override {
        return attribute_delegate().get(t_attr);
    }

    [[nodiscard]] const Expr<Var, Var> &get(const Attribute<Expr<Var, Var>> &t_attr) const override {
        return attribute_delegate().get(t_attr);
    }

    [[nodiscard]] const Constant &get(const Attribute<Constant> &t_attr) const override {
        return attribute_delegate().get(t_attr);
    }

    [[nodiscard]] const Constant &get(const Attribute<Constant> &t_attr, const Ctr &t_ctr, const Var &t_var) const override {
        return attribute_delegate(t_ctr).get(t_attr, t_ctr, t_var);
    }
};

#endif //IDOL_ATTRIBUTEMANAGER_DELEGATE_H
