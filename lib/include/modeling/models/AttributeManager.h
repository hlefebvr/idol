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
    virtual void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var, int t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var, Column&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var, const Column& t_value);
    virtual void set(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var, Constant&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var, const Constant& t_value);

    // Ctr
    virtual void set(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr, int t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr, Row&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr, const Row& t_value);
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, const Constant& t_value);

    // Model
    virtual void set(const AttributeWithTypeAndArguments<int, void>& t_attr, int t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, const LinExpr<Ctr>& t_value);
    virtual void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, const Expr<Var, Var>& t_value);
    virtual void set(const AttributeWithTypeAndArguments<Constant, void>& t_attr, Constant&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Constant, void>& t_attr, const Constant& t_value);
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) = 0;
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, const Constant& t_value);

    /// Get
    // Var
    [[nodiscard]] virtual double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual const Column& get(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var) const = 0;
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var) const = 0;

    // Ctr
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual const Row& get(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr) const = 0;

    // Model
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, void>& t_attr) const = 0;
    [[nodiscard]] virtual const LinExpr<Ctr>& get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr) const = 0;
    [[nodiscard]] virtual const Expr<Var, Var>& get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr) const = 0;
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, void>& t_attr) const = 0;
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const = 0;
};

#endif //IDOL_ATTRIBUTEMANAGER_H
