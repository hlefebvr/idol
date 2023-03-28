//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MODEL_H
#define IDOL_MODEL_H

#include <vector>
#include <string>
#include <functional>

#include "containers/Vector.h"

#include "modeling/matrix/Matrix.h"
#include "modeling/constraints/CtrVersion.h"
#include "modeling/variables/VarVersion.h"
#include "modeling/expressions/Expr.h"

#include "AbstractModel.h"

#include "optimizers/parameters/Timer.h"

static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

class Env;
class Column;
class TempCtr;

/**
 * This class is used to represent a mathematical optimization model.
 */
class Model : public AbstractModel, public Matrix {
    Env& m_env;
    const unsigned int m_id;

    int m_sense = Minimize;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;

    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    // Matrix hooks
    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;
protected:
    // Attribute delegate
    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;
    // Parameter delegate
    AttributeManager &parameter_delegate(const Parameter<double> &t_param) override;
    AttributeManager &parameter_delegate(const Parameter<int> &t_param) override;
    AttributeManager &parameter_delegate(const Parameter<bool> &t_param) override;
public:
    explicit Model(Env& t_env);

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model() override;

    // Variables
    void add(const Var& t_var) override;
    void add(const Var& t_var, TempVar&& t_temp_var) override;
    [[nodiscard]] bool has(const Var& t_var) const override;
    void remove(const Var& t_var) override;
    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const override { return m_variables; }

    // Constraints
    void add(const Ctr& t_ctr) override;
    void add(const Ctr &t_ctr, TempCtr &&t_temp_ctr) override;
    [[nodiscard]] bool has(const Ctr& t_ctr) const override;
    void remove(const Ctr& t_ctr) override;
    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const override { return m_constraints; }

    // Model
    [[nodiscard]] unsigned int id() const override { return m_id; }
    [[nodiscard]] Model* clone() const override;
    [[nodiscard]] Env& env() const override { return const_cast<Model*>(this)->m_env; }

    using AttributeManagers::Delegate::set;
    using AttributeManagers::Delegate::get;

    // Models' attributes
    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;
    [[nodiscard]] const Constant& get(const Req<Constant, void>& t_attr) const override;
    [[nodiscard]] const Expr<Var, Var>& get(const Req<Expr<Var, Var>, void>& t_attr) const override;
    [[nodiscard]] const LinExpr<Ctr>& get(const Req<LinExpr<Ctr>, void>& t_attr) const override;
    [[nodiscard]] const Constant& get(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const override;
    void set(const Req<int, void> &t_attr, int t_value) override;
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override;
    void set(const Req<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override;
    void set(const Req<Constant, void> &t_attr, Constant &&t_value) override;
    void set(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override;

    // Constraints' attributes
    [[nodiscard]] int get(const Req<int, Ctr>& t_attr, const Ctr& t_ctr) const override;
    [[nodiscard]] const Row& get(const Req<Row, Ctr>& t_attr, const Ctr& t_ctr) const override;
    [[nodiscard]] const Constant& get(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr) const override;
    void set(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    void set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
    void set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override;

    // Variables' attributes
    [[nodiscard]] int get(const Req<int, Var>& t_attr, const Var& t_ctr) const override;
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] const Column& get(const Req<Column, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] const Constant& get(const Req<Constant, Var>& t_attr, const Var& t_var) const override;
    void set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) override;
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override;
    void set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override;
    void set(const Req<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override;
};

#endif //IDOL_MODEL_H
