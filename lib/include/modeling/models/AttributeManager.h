//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTEMANAGER_H
#define IDOL_ATTRIBUTEMANAGER_H

#include "Attributes.h"
#include "../../errors/NotImplemented.h"

class Var;
class Ctr;
class Constant;
class Row;
class Column;
template<class> class LinExpr;
template<class Key1, class Key2> class Expr;

class AttributeManager {
protected:
    /// Set
    // Var
    virtual void set_attr_var_double(const Attribute<double>& t_attr, const Var& t_var, double t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_var_int(const Attribute<int>& t_attr, const Var& t_var, int t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_var_Column(const Attribute<Column>& t_attr, const Var& t_var, Column&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_var_Constant(const Attribute<Constant>& t_attr, const Var& t_var, Constant&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    // Ctr
    virtual void set_attr_ctr_int(const Attribute<int>& t_attr, const Ctr& t_ctr, int t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_ctr_Row(const Attribute<Row>& t_attr, const Ctr& t_ctr, Row&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_ctr_Constant(const Attribute<Constant>& t_attr, const Ctr& t_ctr, Constant&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    // Model
    virtual void set_attr_int(const Attribute<int>& t_attr, int t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_LinExpr_Ctr(const Attribute<LinExpr<Ctr>>& t_attr, LinExpr<Ctr>&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_Expr_Var_Var(const Attribute<Expr<Var, Var>>& t_attr, Expr<Var, Var>&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_Constant(const Attribute<Constant>& t_attr, Constant&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    virtual void set_attr_ctr_var_Constant(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    /// Get
    // Var
    [[nodiscard]] virtual double get_attr_var_double(const Attribute<double>& t_attr, const Var& t_var) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual int get_attr_var_int(const Attribute<int>& t_attr, const Var& t_var) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Column& get_attr_var_Column(const Attribute<Column>& t_attr, const Var& t_var) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Constant& get_attr_var_Constant(const Attribute<Constant>& t_attr, const Var& t_var) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }


    // Ctr
    [[nodiscard]] virtual int get_attr_ctr_int(const Attribute<int>& t_attr, const Ctr& t_ctr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Row& get_attr_ctr_Row(const Attribute<Row>& t_attr, const Ctr& t_ctr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Constant& get_attr_ctr_Constant(const Attribute<Constant>& t_attr, const Ctr& t_ctr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }


    // Model
    [[nodiscard]] virtual int get_attr_int(const Attribute<int>& t_attr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const LinExpr<Ctr>& get_attr_LinExpr_Ctr(const Attribute<LinExpr<Ctr>>& t_attr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Expr<Var, Var>& get_attr_Expr_Var_Var(const Attribute<Expr<Var, Var>>& t_attr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Constant& get_attr_Constant(const Attribute<Constant>& t_attr) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }

    [[nodiscard]] virtual const Constant& get_attr_ctr_var_Constant(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var) const {
        throw Exception("Bad request. (Attribute: " + t_attr.name() + ")");
    }
public:
    virtual ~AttributeManager() = default;

    /// Set
    // Var
    void set(const Attribute<double>& t_attr, const Var& t_var, double t_value);
    void set(const Attribute<int>& t_attr, const Var& t_var, int t_value);
    void set(const Attribute<Column>& t_attr, const Var& t_var, const Column& t_value);
    void set(const Attribute<Column>& t_attr, const Var& t_var, Column&& t_value);
    void set(const Attribute<Constant>& t_attr, const Var& t_var, const Constant& t_value);
    void set(const Attribute<Constant>& t_attr, const Var& t_var, Constant&& t_value);

    // Ctr
    void set(const Attribute<int>& t_attr, const Ctr& t_ctr, int t_value);
    void set(const Attribute<Row>& t_attr, const Ctr& t_ctr, const Row& t_value);
    void set(const Attribute<Row>& t_attr, const Ctr& t_ctr, Row&& t_value);
    void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Constant& t_value);
    void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, Constant&& t_value);

    // Model
    void set(const Attribute<int>& t_attr, int t_value);
    void set(const Attribute<LinExpr<Ctr>>& t_attr, const LinExpr<Ctr>& t_value);
    void set(const Attribute<LinExpr<Ctr>>& t_attr, LinExpr<Ctr>&& t_value);
    void set(const Attribute<Expr<Var, Var>>& t_attr, const Expr<Var, Var>& t_value);
    void set(const Attribute<Expr<Var, Var>>& t_attr, Expr<Var, Var>&& t_value);
    void set(const Attribute<Constant>& t_attr, const Constant& t_value);
    void set(const Attribute<Constant>& t_attr, Constant&& t_value);
    void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var, const Constant& t_value);
    void set(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value);

    /// Get
    // Var
    [[nodiscard]] double get(const Attribute<double>& t_attr, const Var& t_var) const;
    [[nodiscard]] int get(const Attribute<int>& t_attr, const Var& t_var) const;
    [[nodiscard]] const Column& get(const Attribute<Column>& t_attr, const Var& t_var) const;
    [[nodiscard]] const Constant& get(const Attribute<Constant>& t_attr, const Var& t_var) const;

    // Ctr
    [[nodiscard]] int get(const Attribute<int>& t_attr, const Ctr& t_ctr) const;
    [[nodiscard]] const Row& get(const Attribute<Row>& t_attr, const Ctr& t_ctr) const;
    [[nodiscard]] const Constant& get(const Attribute<Constant>& t_attr, const Ctr& t_ctr) const;

    // Model
    [[nodiscard]] int get(const Attribute<int>& t_attr) const;
    [[nodiscard]] const LinExpr<Ctr>& get(const Attribute<LinExpr<Ctr>>& t_attr) const;
    [[nodiscard]] const Expr<Var, Var>& get(const Attribute<Expr<Var, Var>>& t_attr) const;
    [[nodiscard]] const Constant& get(const Attribute<Constant>& t_attr) const;
    [[nodiscard]] const Constant& get(const Attribute<Constant>& t_attr, const Ctr& t_ctr, const Var& t_var) const;

};

#endif //IDOL_ATTRIBUTEMANAGER_H
