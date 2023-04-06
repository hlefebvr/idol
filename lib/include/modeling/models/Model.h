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
#include "modeling/solutions/Solution.h"

#include "Model.h"

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
class Model : public Matrix {
    Env& m_env;
    const unsigned int m_id;

    ObjectiveSense m_sense = Minimize;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;

    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    std::unique_ptr<Optimizer> m_optimizer;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    void throw_if_no_optimizer() const { if (!m_optimizer) { throw Exception("No optimizer was found."); } }

    // Matrix hooks
    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;
public:
    explicit Model(Env& t_env);

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model() override;

    // Variables
    Var add_var(double t_lb, double t_ub, VarType t_type, std::string t_name = "");
    Var add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    template<unsigned int N> Vector<Var, N> add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, const std::string& t_name = "");
    void add(const Var& t_var);
    void add(const Var& t_var, TempVar t_temp_var);
    [[nodiscard]] bool has(const Var& t_var) const;
    void remove(const Var& t_var);
    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const { return m_variables; }

    // Constraints
    Ctr add_ctr(TempCtr t_temp_ctr, std::string t_name = "");
    Ctr add_ctr(Row&& t_row, CtrType t_type, std::string t_name = "");
    template<unsigned int N> Vector<Ctr, N> add_ctrs(Dim<N> t_dim, CtrType t_type, const Constant& t_constant, const std::string& t_name = "");
    void add(const Ctr& t_ctr);
    void add(const Ctr &t_ctr, TempCtr t_temp_ctr);
    [[nodiscard]] bool has(const Ctr& t_ctr) const;
    void remove(const Ctr& t_ctr);
    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const { return m_constraints; }

    // Model
    [[nodiscard]] unsigned int id() const { return m_id; }
    [[nodiscard]] Model* clone() const;
    [[nodiscard]] Env& env() const { return const_cast<Model*>(this)->m_env; }

    template<class T, unsigned int N> void add_vector(const Vector<T, N>& t_vector);

    // Optimizer
    Optimizer& optimizer() { throw_if_no_optimizer(); return *m_optimizer; }
    [[nodiscard]] const Optimizer& optimizer() const { throw_if_no_optimizer(); return *m_optimizer; }
    void optimize();
    void write(const std::string& t_name);
    void update();
    void use(const OptimizerFactory& t_optimizer_factory);
    void unuse();
    [[nodiscard]] bool has_optimizer() const;

    // Models' attributes
    [[nodiscard]] ObjectiveSense get_obj_sense() const;
    [[nodiscard]] const Expr<Var, Var>& get_obj_expr() const;
    [[nodiscard]] const LinExpr<Ctr>& get_rhs_expr() const;
    [[nodiscard]] const Constant& get_mat_coeff(const Ctr& t_ctr, const Var& t_var) const;
    [[nodiscard]] SolutionStatus get_status() const;
    [[nodiscard]] SolutionReason get_reason() const;
    [[nodiscard]] double get_best_obj() const;
    [[nodiscard]] double get_best_bound() const;
    void set_obj_sense(ObjectiveSense t_value);
    void set_obj_expr(const Expr<Var, Var>& t_objective);
    void set_obj_expr(Expr<Var, Var>&& t_objective);
    void set_rhs_expr(const LinExpr<Ctr>& t_rhs);
    void set_rhs_expr(LinExpr<Ctr>&& t_rhs);
    void set_obj_const(const Constant& t_constant);
    void set_obj_const(Constant&& t_constant);
    void set_mat_coeff(const Ctr& t_ctr, const Var& t_var, const Constant& t_coeff);
    void set_mat_coeff(const Ctr& t_ctr, const Var& t_var, Constant&& t_coeff);

    // Constraints' attributes
    [[nodiscard]] unsigned int get_ctr_index(const Ctr& t_ctr) const;
    [[nodiscard]] CtrType get_ctr_type(const Ctr& t_ctr) const;
    [[nodiscard]] const Row& get_ctr_row(const Ctr& t_ctr) const;
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const;
    void set_ctr_rhs(const Ctr& t_ctr, const Constant& t_rhs);
    void set_ctr_rhs(const Ctr& t_ctr, Constant&& t_rhs);
    void set_ctr_type(const Ctr& t_ctr, CtrType t_type);
    void set_ctr_row(const Ctr& t_ctr, const Row& t_row);
    void set_ctr_row(const Ctr& t_ctr, Row&& t_row);

    // Variables' attributes
    [[nodiscard]] unsigned int get_var_index(const Var& t_var) const;
    [[nodiscard]] VarType get_var_type(const Var& t_var) const;
    [[nodiscard]] double get_var_lb(const Var& t_var) const;
    [[nodiscard]] double get_var_ub(const Var& t_var) const;
    [[nodiscard]] double get_var_primal(const Var& t_var) const;
    [[nodiscard]] double get_var_ray(const Var& t_var) const;
    [[nodiscard]] const Column& get_var_column(const Var& t_var) const;
    void set_var_type(const Var& t_var, VarType t_type);
    void set_var_lb(const Var& t_var, double t_lb);
    void set_var_ub(const Var& t_var, double t_ub);
    void set_var_obj(const Var& t_var, const Constant& t_obj);
    void set_var_obj(const Var& t_var, Constant&& t_obj);
    void set_var_column(const Var& t_var, const Column& t_column);
    void set_var_column(const Var& t_var, Column&& t_column);
};

template<class T, unsigned int N>
void Model::add_vector(const Vector<T, N> &t_vector) {
    if constexpr (N == 1) {
        for (const auto& x : t_vector) {
            add(x);
        }
    } else  {
        for (const auto& x : t_vector) {
            add_vector<T, N - 1>(x);
        }
    }
}

template<unsigned int N>
Vector<Var, N> Model::add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, const std::string& t_name) {
    auto result = Var::make_vector(m_env, t_dim, t_lb, t_ub, t_type, t_name);
    add_vector<Var, N>(result);
    return result;
}

template<unsigned int N>
Vector<Ctr, N> Model::add_ctrs(Dim<N> t_dim, CtrType t_type, const Constant &t_constant, const std::string &t_name) {
    auto result = Ctr::make_vector(m_env, t_dim, t_type, t_constant, t_name);
    add_vector<Ctr, N>(result);
    return result;
}

static auto save_primal(const Model& t_original_model, const Model& t_model) {

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
        result.set(var, t_model.get_var_primal(var));
    }

    return result;

}

static auto save_primal(const Model& t_original_model) {
    return save_primal(t_original_model, t_original_model);
}

static auto save_ray(const Model& t_original_model, const Model& t_model) {

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

static auto save_ray(const Model& t_original_model) {
    return save_ray(t_original_model, t_original_model);
}

static auto save_dual(const Model& t_original_model, const Model& t_model) {

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
        result.set(ctr, t_model.get_ctr_dual(ctr));
    }

    return result;

}

static auto save_dual(const Model& t_original_model) {
    return save_dual(t_original_model, t_original_model);
}

static auto save_farkas(const Model& t_original_model, const Model& t_model) {

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

static auto save_farkas(const Model& t_original_model) {
    return save_farkas(t_original_model, t_original_model);
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {

    if (t_model.get_obj_sense() == Minimize) {
        t_os << "Minimize";
    } else {
        t_os << "Maximize";
    }

    t_os << " " << t_model.get_obj_expr() << "\nSubject to:\n";
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
