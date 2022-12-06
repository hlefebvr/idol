//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGERS_BASE_H
#define IDOL_ATTRIBUTEMANAGERS_BASE_H

#include "AttributeManager.h"
#include "../../errors/Exception.h"
#include "errors/AttributeBadRequest.h"

namespace AttributeManagers {
    class Base;
}

class AttributeManagers::Base : public AttributeManager {
public:
    using AttributeManager::set;
    using AttributeManager::get;

    /// Set
    // Var
    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var, int t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var, Column&& t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var, Constant&& t_value) override { throw AttributeBadRequest(t_attr); }

    // Ctr
    void set(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr, int t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr, Row&& t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override { throw AttributeBadRequest(t_attr); }

    // Model
    void set(const AttributeWithTypeAndArguments<int, void>& t_attr, int t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Constant, void>& t_attr, Constant&& t_value) override { throw AttributeBadRequest(t_attr); }
    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override { throw AttributeBadRequest(t_attr); }

    /// Get
    // Var
    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Column& get(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var) const override { throw AttributeBadRequest(t_attr); }

    // Ctr
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Row& get(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr) const override { throw AttributeBadRequest(t_attr); }

    // Model
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, void>& t_attr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const LinExpr<Ctr>& get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Expr<Var, Var>& get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, void>& t_attr) const override { throw AttributeBadRequest(t_attr); }
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const override { throw AttributeBadRequest(t_attr); }

};

#endif //IDOL_ATTRIBUTEMANAGERS_BASE_H
