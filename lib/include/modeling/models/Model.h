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

    // Models' attributes
    [[nodiscard]] int get_obj_sense() const;
    [[nodiscard]] const Expr<Var, Var>& get_obj() const;
    [[nodiscard]] const LinExpr<Ctr>& get_rhs() const;
    [[nodiscard]] const Constant& get_mat_coeff(const Ctr& t_ctr, const Var& t_var) const;
    [[nodiscard]] int get_status() const;
    [[nodiscard]] int get_reason() const;
    [[nodiscard]] double get_best_obj() const;
    [[nodiscard]] double get_best_bound() const;
    void set(const Req<int, void> &t_attr, int t_value) override;
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override;
    void set(const Req<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override;
    void set(const Req<Constant, void> &t_attr, Constant &&t_value) override;
    void set(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override;

    // Constraints' attributes
    [[nodiscard]] unsigned int get_ctr_index(const Ctr& t_ctr) const;
    [[nodiscard]] int get_ctr_type(const Ctr& t_ctr) const;
    [[nodiscard]] const Row& get_ctr_row(const Ctr& t_ctr) const;
    [[nodiscard]] double get_ctr_val(const Ctr& t_ctr) const;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const;
    void set(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    void set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
    void set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override;

    // Variables' attributes
    [[nodiscard]] unsigned int get_var_index(const Var& t_var) const;
    [[nodiscard]] int get_var_type(const Var& t_var) const;
    [[nodiscard]] double get_var_lb(const Var& t_var) const;
    [[nodiscard]] double get_var_ub(const Var& t_var) const;
    [[nodiscard]] double get_var_val(const Var& t_var) const;
    [[nodiscard]] double get_var_ray(const Var& t_var) const;
    [[nodiscard]] const Column& get_var_column(const Var& t_var) const;
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

static auto save_primal_values(const Model& t_original_model, const Model& t_model) {

    Solution::Primal result;

    const auto status = t_model.get_status();
    const auto reason = t_model.get_reason();

    if (status != Optimal && status != Feasible) {
        throw Exception("Not available.");
    }

    result.set_status(status);
    result.set_reason(reason);

    result.set_objective_value(t_model.get_best_obj());

    for (const auto& var : t_original_model.vars()) {
        result.set(var, t_model.get_var_val(var));
    }

    return result;

}

static auto save_primal_values(const Model& t_original_model) {
    return save_primal_values(t_original_model, t_original_model);
}

static auto save_ray_values(const Model& t_original_model, const Model& t_model) {

    Solution::Primal result;

    const auto status = t_model.get_status();
    const auto reason = t_model.get_reason();

    if (status != Unbounded) {
        throw Exception("Not available.");
    }

    result.set_status(status);
    result.set_reason(reason);

    result.set_objective_value(-Inf);

    for (const auto& var : t_original_model.vars()) {
        result.set(var, t_model.get_var_ray(var));
    }

    return result;

}

static auto save_ray_values(const Model& t_original_model) {
    return save_ray_values(t_original_model, t_original_model);
}

static auto save_dual_values(const Model& t_original_model, const Model& t_model) {

    Solution::Dual result;

    const auto status = t_model.get_status();
    const auto reason = t_model.get_reason();

    if (status != Optimal && status != Feasible) {
        throw Exception("Not available.");
    }

    result.set_status(status);
    result.set_reason(reason);

    result.set_objective_value(t_model.get_best_bound());

    for (const auto& ctr : t_original_model.ctrs()) {
        result.set(ctr, t_model.get_ctr_val(ctr));
    }

    return result;

}

static auto save_dual_values(const Model& t_original_model) {
    return save_dual_values(t_original_model, t_original_model);
}

static auto save_farkas_values(const Model& t_original_model, const Model& t_model) {

    Solution::Dual result;

    const auto status = t_model.get_status();
    const auto reason = t_model.get_reason();

    if (status != Infeasible) {
        throw Exception("Not available.");
    }

    result.set_status(status);
    result.set_reason(reason);

    result.set_objective_value(+Inf);

    for (const auto& ctr : t_original_model.ctrs()) {
        result.set(ctr, t_model.get_ctr_farkas(ctr));
    }

    return result;

}

static auto save_farkas_values(const Model& t_original_model) {
    return save_farkas_values(t_original_model, t_original_model);
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {

    if (t_model.get_obj_sense() == Minimize) {
        t_os << "Minimize";
    } else {
        t_os << "Maximize";
    }

    t_os << " " << t_model.get_obj() << "\nSubject to:\n";
    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get_ctr_row(ctr);
        const auto& linear = row.linear();
        const auto& quadratic = row.quadratic();
        const auto type = t_model.get_ctr_type(ctr);

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

        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);
        const int type = t_model.get_var_type(var);

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
