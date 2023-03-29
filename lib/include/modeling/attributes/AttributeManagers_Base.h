//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGERS_BASE_H
#define IDOL_ATTRIBUTEMANAGERS_BASE_H

#include "AttributeManager.h"
#include "../../errors/Exception.h"
#include "errors/UnsupportedRequest.h"

namespace AttributeManagers {
    class Base;
}

class AttributeManagers::Base : public AttributeManager {
public:
    using AttributeManager::set;
    using AttributeManager::get;

    //// ATTRIBUTES

    /// Set
    // Var
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<int, Var>& t_attr, const Var& t_var, int t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Column, Var>& t_attr, const Var& t_var, Column&& t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Constant, Var>& t_attr, const Var& t_var, Constant&& t_value) override { throw UnsupportedRequest(t_attr); }

    // Ctr
    void set(const Req<int, Ctr>& t_attr, const Ctr& t_ctr, int t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<double, Ctr>& t_attr, const Ctr& t_ctr, double t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Row, Ctr>& t_attr, const Ctr& t_ctr, Row&& t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override { throw UnsupportedRequest(t_attr); }

    // Model
    void set(const Req<int, void>& t_attr, int t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Constant, void>& t_attr, Constant&& t_value) override { throw UnsupportedRequest(t_attr); }
    void set(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override { throw UnsupportedRequest(t_attr); }

    /// Get
    // Var
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] int get(const Req<int, Var>& t_attr, const Var& t_var) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Column& get(const Req<Column, Var>& t_attr, const Var& t_var) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Constant& get(const Req<Constant, Var>& t_attr, const Var& t_var) const override { throw UnsupportedRequest(t_attr); }

    // Ctr
    [[nodiscard]] int get(const Req<int, Ctr>& t_attr, const Ctr& t_ctr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] double get(const Req<double, Ctr>& t_attr, const Ctr& t_ctr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Row& get(const Req<Row, Ctr>& t_attr, const Ctr& t_ctr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Constant& get(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr) const override { throw UnsupportedRequest(t_attr); }

    // Model
    [[nodiscard]] double get(const Req<double, void>& t_attr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] int get(const Req<int, void>& t_attr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const LinExpr<Ctr>& get(const Req<LinExpr<Ctr>, void>& t_attr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Expr<Var, Var>& get(const Req<Expr<Var, Var>, void>& t_attr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Constant& get(const Req<Constant, void>& t_attr) const override { throw UnsupportedRequest(t_attr); }
    [[nodiscard]] const Constant& get(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const override { throw UnsupportedRequest(t_attr); }
};

#endif //IDOL_ATTRIBUTEMANAGERS_BASE_H
