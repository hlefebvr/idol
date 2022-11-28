//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGER_H
#define IDOL_ATTRIBUTEMANAGER_H

#include "Attributes.h"

class Var;
class Ctr;
class Constant;
class Row;
class Column;
template<class> class LinExpr;
template<class Key1, class Key2> class Expr;

class AttributeManager {
public:
    virtual ~AttributeManager() = default;

    /// Set
    // Var
    virtual void set(const Attribute<double>& t_attr, const Var& t_var, double t_value) = 0;
    virtual void set(const Attribute<int>& t_attr, const Var& t_var, int t_value) = 0;
    virtual void set(const Attribute<Column>& t_attr, const Var& t_var, const Column& t_value) = 0;
    virtual void set(const Attribute<Column>& t_attr, const Var& t_var, Column&& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Var& t_var, const Constant& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Var& t_var, Constant&& t_value) = 0;

    // Ctr
    virtual void set(const Attribute<int>& t_attr, const Ctr& t_ctr, int t_value) = 0;
    virtual void set(const Attribute<Row>& t_attr, const Ctr& t_ctr, const Row& t_value) = 0;
    virtual void set(const Attribute<Row>& t_attr, const Ctr& t_ctr, Row&& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Constant& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, Constant&& t_value) = 0;

    // Model
    virtual void set(const Attribute<int>& t_attr, int t_value) = 0;
    virtual void set(const Attribute<LinExpr<Ctr>>& t_attr, const LinExpr<Ctr>& t_value) = 0;
    virtual void set(const Attribute<LinExpr<Ctr>>& t_attr, LinExpr<Ctr>&& t_value) = 0;
    virtual void set(const Attribute<Expr<Var, Var>>& t_attr, const Expr<Var, Var>& t_value) = 0;
    virtual void set(const Attribute<Expr<Var, Var>>& t_attr, Expr<Var, Var>&& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Constant& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, Constant&& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var, const Constant& t_value) = 0;
    virtual void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) = 0;

    /// Get
    // Var
    [[nodiscard]] virtual double get(const Attribute<double>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual int get(const Attribute<int>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual const Column& get(const Attribute<Column>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual const Constant& get(const Attribute<Constant>& t_attr, const Var& t_var) const = 0;

    // Ctr
    [[nodiscard]] virtual int get(const Attribute<int>& t_attr, const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual const Row& get(const Attribute<Row>& t_attr, const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual const Constant& get(const Attribute<Constant>& t_attr, const Ctr& t_ctr) const = 0;

    // Model
    [[nodiscard]] virtual int get(const Attribute<int>& t_attr) const = 0;
    [[nodiscard]] virtual const LinExpr<Ctr>& get(const Attribute<LinExpr<Ctr>>& t_attr) const = 0;
    [[nodiscard]] virtual const Expr<Var, Var>& get(const Attribute<Expr<Var, Var>>& t_attr) const = 0;
    [[nodiscard]] virtual const Constant& get(const Attribute<Constant>& t_attr) const = 0;
    [[nodiscard]] virtual const Constant& get(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var) const = 0;
};

#endif //IDOL_ATTRIBUTEMANAGER_H
