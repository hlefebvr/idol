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

#include "Model.h"

#include "modeling/attributes/AttributeManager_Delegate.h"
#include "Attributes_Model.h"
#include "../constraints/Attributes_Ctr.h"
#include "../variables/Attributes_Var.h"

#include "optimizers/Optimizer.h"
#include "optimizers/OptimizerFactory.h"

#include "optimizers/Timer.h"

static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

class Env;
class Column;
class TempCtr;

/**
 * This class is used to represent a mathematical optimization model.
 */
class Model : public AttributeManagers::Delegate, public Matrix {
    Env& m_env;
    const unsigned int m_id;

    int m_sense = Minimize;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;

    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    std::unique_ptr<Optimizer> m_optimizer;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    // Matrix hooks
    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;
protected:
    void throw_if_no_optimizer() const { if (!m_optimizer) { throw Exception("No backend was found."); } }
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
    void add(const Var& t_var);
    void add(const Var& t_var, TempVar&& t_temp_var);
    [[nodiscard]] bool has(const Var& t_var) const;
    void remove(const Var& t_var);
    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const { return m_variables; }

    // Constraints
    void add(const Ctr& t_ctr);
    void add(const Ctr &t_ctr, TempCtr &&t_temp_ctr);
    [[nodiscard]] bool has(const Ctr& t_ctr) const;
    void remove(const Ctr& t_ctr);
    [[nodiscard]] auto ctrs() const { return ConstIteratorForward(m_constraints); }

    // Model
    [[nodiscard]] unsigned int id() const { return m_id; }
    [[nodiscard]] Model* clone() const;
    [[nodiscard]] Env& env() const { return const_cast<Model*>(this)->m_env; }

    template<class T, unsigned int N> void add_array(const Vector<T, N>& t_vector);
    template<class T, class ...ArgsT> void add_many(const T& t_object, const ArgsT& ...t_args);

    // Optimizer
    Optimizer& optimizer() { throw_if_no_optimizer(); return *m_optimizer; }
    [[nodiscard]] const Optimizer& optimizer() const { throw_if_no_optimizer(); return *m_optimizer; }
    void optimize();
    void write(const std::string& t_name);
    void update();
    void use(const OptimizerFactory& t_optimizer_factory);
    void unuse();
    [[nodiscard]] bool has_optimizer() const;

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

template<class T, class... ArgsT>
void Model::add_many(const T &t_object, const ArgsT &... t_args) {
    add(t_object);
    if constexpr (sizeof...(t_args) > 0) {
        add_many(t_args...);
    }
}

template<class T, unsigned int N>
void Model::add_array(const Vector<T, N> &t_vector) {
    if constexpr (N == 1) {
        for (const auto& x : t_vector) {
            add(x);
        }
    } else  {
        for (const auto& x : t_vector) {
            add_array<T, N - 1>(x);
        }
    }
}

template<class ObjectT>
auto save(const Model& t_original_model, const Req<double, ObjectT>& t_attr, const Model& t_model) {

    std::conditional_t<std::is_same_v<ObjectT, Var>, Solution::Primal, Solution::Dual> result;

    const int sense = t_original_model.get(Attr::Obj::Sense);
    const int status = t_model.get(Attr::Solution::Status);
    const int reason = t_model.get(Attr::Solution::Reason);

    result.set_status(status);
    result.set_reason(reason);

    if (status == Infeasible) {

        result.set_objective_value(sense == Minimize ? Inf : -Inf);

        if constexpr (std::is_same_v<ObjectT, Var>) {
            if (t_attr == Attr::Solution::Primal) {
                return result;
            }
        }

        if constexpr (std::is_same_v<ObjectT, Ctr>) {
            if (t_attr == Attr::Solution::Dual) {
                return result;
            }
        }

    } else if (status == Unbounded) {

        result.set_objective_value(sense == Minimize ? -Inf : Inf);

        if constexpr (std::is_same_v<ObjectT, Var>) {
            if (t_attr == Attr::Solution::Primal) {
                return result;
            }
        }

        if constexpr (std::is_same_v<ObjectT, Ctr>) {
            if (t_attr == Attr::Solution::Dual) {
                return result;
            }
        }

    } else {

        result.set_objective_value(t_model.get(Attr::Solution::ObjVal));

    }

    if constexpr (std::is_same_v<ObjectT, Var>) {
        for (const auto &var: t_original_model.vars()) {
            result.set(var, t_model.get(t_attr, var));
        }
    } else {
        for (const auto &ctr: t_original_model.ctrs()) {
            result.set(ctr, t_model.get(t_attr, ctr));
        }
    }

    return result;
}

template<class ObjectT>
auto save(const Model& t_model, const Req<double, ObjectT>& t_attr) {
    return save(t_model, t_attr, t_model);
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {

    if (t_model.get(Attr::Obj::Sense) == Minimize) {
        t_os << "Minimize";
    } else {
        t_os << "Maximize";
    }

    t_os << " " << t_model.get(Attr::Obj::Expr) << "\nSubject to:\n";
    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get(Attr::Ctr::Row, ctr);
        const auto& linear = row.linear();
        const auto& quadratic = row.quadratic();
        const auto type = t_model.get(Attr::Ctr::Type, ctr);

        t_os << ctr << ": ";

        if (linear.empty()) {
            t_os << quadratic;
        } else {
            t_os << linear;
            if (!quadratic.empty()) {
                t_os << " + " << quadratic;
            }
        }

        switch (type) {
            case LessOrEqual: t_os << " <= "; break;
            case Equal: t_os << " = "; break;
            case GreaterOrEqual: t_os << " >= "; break;
            default: t_os << " ?undefined? ";
        }

        t_os << row.rhs() << '\n';
    }

    t_os << "Variables:\n";
    for (const auto& var : t_model.vars()) {

        const double lb = t_model.get(Attr::Var::Lb, var);
        const double ub = t_model.get(Attr::Var::Ub, var);
        const int type = t_model.get(Attr::Var::Type, var);

        if (!is_neg_inf(lb) && !is_pos_inf(ub)) {
            t_os << lb << " <= " << var << " <= " << ub;
        } else if (!is_pos_inf(ub)) {
            t_os << var << " <= " << ub;
        } else if (!is_neg_inf(lb)) {
            t_os << var << " >= " << lb;
        } else {
            t_os << var;
        }

        if (type == Binary) {
            t_os << " [binary]";
        } else if (type == Integer) {
            t_os << " [integer]";
        }

        t_os << '\n';
    }
    return t_os;
}

#endif //IDOL_MODEL_H
