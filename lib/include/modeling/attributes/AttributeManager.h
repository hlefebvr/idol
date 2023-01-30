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

/**
 * An AttributeManager is a class which stores and updates attributes of modeling objects.
 * Its interface is based on get and set functions which present a unified way of accessing and modifying
 * attributes. In idol, Model and child classes of Algorithm are AttributeManager-s.
 *
 * **Example**:
 * ```
 * Model model;
 * Var x = model.add_var(0., 1., Binary, 0., "x");
 *
 * Expr objective = model.get(Attr::Obj::Expr); // Accesses the objective
 *
 * double lb = model.get(Attr::Var::Lb, x);
 *
 * Solvers::GLPK solver(model);
 * solver.set(Attr::Var::Ub, x, 0.);
 * ```
 */
class AttributeManager {
public:
    virtual ~AttributeManager() = default;

    /// Set
    // Var

    /**
     * Sets the value of an attribute of type double with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) = 0;

    /**
     * Sets the value of an attribute of type int with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var, int t_value) = 0;

    /**
     * Sets the value of an attribute of type Column with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var, Column&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Column with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var, const Column& t_value);

    /**
     * Sets the value of an attribute of type Constant with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var, Constant&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Constant with a Var argument
     * @param t_attr the attribute to update
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var, const Constant& t_value);

    // Ctr

    /**
     * Sets the value of an attribute of type int with a Ctr argument
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr, int t_value) = 0;

    /**
     * Sets the value of an attribute of type Row with a Ctr argument
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr, Row&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Row with a Ctr argument
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr, const Row& t_value);

    /**
     * Sets the value of an attribute of type Constant with a Ctr argument
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Constant with a Ctr argument
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, const Constant& t_value);

    // Model

    /**
     * Sets the value of an attribute of type int
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<int, void>& t_attr, int t_value) = 0;

    /**
     * Sets the value of an attribute of type LinExpr<Ctr>
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) = 0;

    /**
     * Sets the value of an attribute of type LinExpr<Ctr>
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, const LinExpr<Ctr>& t_value);

    /**
     * Sets the value of an attribute of type Expr<Var, Var>
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Expr<Var, Var>
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, const Expr<Var, Var>& t_value);

    /**
     * Sets the value of an attribute of type Constant
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, void>& t_attr, Constant&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Constant
     * @param t_attr the attribute to update
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, void>& t_attr, const Constant& t_value);

    /**
     * Sets the value of an attribute of type Constant with arguments of type Ctr and Var
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) = 0;

    /**
     * Sets the value of an attribute of type Constant with arguments of type Ctr and Var
     * @param t_attr the attribute to update
     * @param t_ctr the constraint argument
     * @param t_var the variable argument
     * @param t_value the new value
     */
    virtual void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, const Constant& t_value);

    /// Get
    // Var

    /**
     * Returns the value of an attribute of type double with argument of type Var
     * @param t_attr the attribute to retrieve
     * @param t_var the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const = 0;

    /**
     * Returns the value of an attribute of type int with argument of type Var
     * @param t_attr the attribute to retrieve
     * @param t_var the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var) const = 0;

    /**
     * Returns the value of an attribute of type Column with argument of type Var
     * @param t_attr the attribute to retrieve
     * @param t_var the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Column& get(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var) const = 0;

    /**
     * Returns the value of an attribute of type Constant with argument of type Var
     * @param t_attr the attribute to retrieve
     * @param t_var the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var) const = 0;

    // Ctr
    /**
     * Returns the value of an attribute of type int with argument of type Ctr
     * @param t_attr the attribute to retrieve
     * @param t_ctr the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr) const = 0;

    /**
     * Returns the value of an attribute of type Row with argument of type Ctr
     * @param t_attr the attribute to retrieve
     * @param t_ctr the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Row& get(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr) const = 0;

    /**
     * Returns the value of an attribute of type Constant with argument of type Ctr
     * @param t_attr the attribute to retrieve
     * @param t_ctr the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr) const = 0;

    // Model
    /**
     * Returns the value of an attribute of type double
     * @param t_attr the attribute to retrieve
     * @return the value of the attribute
     */
    [[nodiscard]] virtual double get(const AttributeWithTypeAndArguments<double, void>& t_attr) const = 0;

    /**
     * Returns the value of an attribute of type int
     * @param t_attr the attribute to retrieve
     * @return the value of the attribute
     */
    [[nodiscard]] virtual int get(const AttributeWithTypeAndArguments<int, void>& t_attr) const = 0;

    /**
     * Returns the value of an attribute of type LinExpr<Ctr>
     * @param t_attr the attribute to retrieve
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const LinExpr<Ctr>& get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr) const = 0;

    /**
     * Returns the value of an attribute of type Expr<Var, Var>
     * @param t_attr the attribute to retrieve
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Expr<Var, Var>& get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr) const = 0;

    /**
     * Returns the value of an attribute of type Constant
     * @param t_attr the attribute to retrieve
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, void>& t_attr) const = 0;

    /**
     * Returns the value of an attribute of type Constant with arguments of type Ctr and Var
     * @param t_attr the attribute to retrieve
     * @param t_ctr the constraint argument
     * @param t_var the variable argument
     * @return the value of the attribute
     */
    [[nodiscard]] virtual const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const = 0;
};

#endif //IDOL_ATTRIBUTEMANAGER_H
