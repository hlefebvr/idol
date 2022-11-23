//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include "../environment/ObjectManager.h"
#include "../variables/impl_Variable.h"
#include "../variables/TempVar.h"
#include "../constraints/impl_Constraint.h"
#include "../constraints/TempCtr.h"
#include "../expressions/Expr.h"
#include "../../containers/IteratorForward.h"
#include "../../containers/Vector.h"
#include "../objective/Objective.h"
#include "ListenerManager.h"
#include "../variables/VarAttributes.h"
#include "ObjectStore.h"
#include "../constraints/CtrAttributes.h"
#include "../matrix/Matrix.h"
#include <vector>

class Env;
class Column;

/**
 * Optimization model object.
 *
 * This class represents an optimization model and is *mainly* composed of attributes (see Param), variables (see Var) and constraints (see Ctr).
 * It is used to represent an optimization problem given in the following form:
 *
 * \f{align}{
 *  \textrm{minimize } & c^Tx \\
 *  \textrm{subject to } & Ax \ge b \\
 *  & x \in \mathbb R \\
 *  & x_j \in \mathbb N \textrm{ for some } j
 * \f}
 */
class Model : public Matrix {
    mutable ListenerManager m_listeners;

    Sense m_objective_sense;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;
    ObjectStore<Var, VarAttributes> m_variables;
    ObjectStore<Ctr, CtrAttributes> m_constraints;

    template<class T, int N, int I = 0> Vector<T, N-I> add_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one);

    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;
public:
    explicit Model(Sense t_sense = Minimize);

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    /* Gets */
    Sense sense() const { return m_objective_sense; }

    const Expr<Var>& obj() const { return m_objective; }

    const LinExpr<Ctr>& rhs() const { return m_rhs; }

    auto vars() { return m_variables.objects(); }
    [[nodiscard]] auto vars() const { return m_variables.objects(); }

    auto ctrs() { return m_constraints.objects(); }
    [[nodiscard]] auto ctrs() const { return m_constraints.objects(); }

    /* Adds */
    Var add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    Var add_var(double t_lb, double t_ub, VarType t_type, Constant t_objective_coefficient, std::string t_name = "");
    Var add_var(TempVar t_temporary_variable, std::string t_name = "");
    template<int N> Vector<Var, N> add_vars(const Dim<N>& t_dim, double t_lb, double t_ub, VarType t_type, const Constant& t_objective_coefficient, const std::string& t_name = "");
    template<int N> Vector<Var, N> add_vars(const Dim<N>& t_dim, const TempVar& t_temporary_variable, const std::string& t_name = "");

    Ctr add_ctr(CtrType t_type, Constant t_rhs, std::string t_name = "");
    Ctr add_ctr(TempCtr t_temporary_constraint, std::string t_name = "");
    template<int N> Vector<Ctr, N> add_ctrs(const Dim<N>& t_dim, CtrType t_type, const Constant& t_rhs, const std::string& t_name = "");
    template<int N> Vector<Ctr, N> add_ctrs(const Dim<N>& t_dim, const TempCtr& t_temporary_constraint, const std::string& t_name = "");

    void add_listener(Listener& t_listener) const;

    /* Updates */
    void update_rhs(LinExpr<Ctr>&& t_obj);
    void update_rhs(const LinExpr<Ctr>& t_obj);
    void update_obj(Expr<Var>&& t_obj);
    void update_obj(const Expr<Var>& t_obj);
    void update_obj_sense(Sense t_sense);
    void update_obj_const(Constant t_offset);
    void update_obj_coeff(const Var& t_var, Constant t_coefficient);

    void update_rhs_coeff(const Ctr& t_ctr, Constant t_coefficient);

    void update_matrix_coeff(const Ctr& t_ctr, const Var& t_var, Constant t_coefficient);

    void update_var_lb(const Var& t_var, double t_lb);
    void update_var_ub(const Var& t_var, double t_ub);
    void update_var_type(const Var& t_var, VarType t_type);

    void update_ctr_type(const Ctr& t_ctr, CtrType t_type);

    /* Removes */
    void remove(const Var& t_var);
    void remove(const Ctr& t_ctr);

    /* Variables */
    double get_lb(const Var& t_var) const;
    double get_ub(const Var& t_var) const;
    VarType get_type(const Var& t_var) const;
    const Column& get_column(const Var& t_var) const;
    bool has(const Var& t_var) const;

    /* Constraints */
    CtrType get_type(const Ctr& t_ctr) const;
    const Row& get_row(const Ctr& t_ctr) const;
    bool has(const Ctr& t_ctr) const;
};

template<class T, int N, int I>
Vector<T, N - I> Model::add_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one) {
    Vector<T, N - I> result;
    const unsigned int size = t_dims[I];
    result.reserve(size);
    for (unsigned int i = 0 ; i < size ; ++i) {
        const std::string name = t_name + "_" + std::to_string(i);
        if constexpr (I == N - 1) {
            result.emplace_back( t_add_one(name) );
        } else {
            result.emplace_back( add_many<T, N, I+1>(t_dims, name, t_add_one) );
        }
    }
    return result;
}

template<int N>
Vector<Var, N> Model::add_vars(const Dim<N> &t_dim, double t_lb, double t_ub, VarType t_type,
                               const Constant &t_objective_coefficient, const std::string& t_name) {
    return add_many<Var, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_var(t_lb, t_ub, t_type, t_objective_coefficient, t_name);
    });
}

template<int N>
Vector<Var, N> Model::add_vars(const Dim<N> &t_dim, const TempVar& t_temporary_variable, const std::string &t_name) {
    return add_many<Var, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_var(TempVar(t_temporary_variable), t_name);
    });
}

template<int N>
Vector<Ctr, N> Model::add_ctrs(const Dim<N> &t_dim, CtrType t_type, const Constant& t_rhs, const std::string &t_name) {
    return add_many<Ctr, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_ctr(t_type, t_rhs, t_name);
    });
}

template<int N>
Vector<Ctr, N>
Model::add_ctrs(const Dim<N> &t_dim, const TempCtr &t_temporary_constraint, const std::string &t_name) {
    return add_many<Ctr, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_ctr(t_temporary_constraint, t_name);
    });
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {
    t_os << t_model.sense() << " " << t_model.obj() << "\nSubject to:\n";
    for (const auto& ctr : t_model.ctrs()) {
        const auto& row = t_model.get_row(ctr);
        t_os << ctr << ": " << row.lhs().linear() << " ? " << row.rhs() << '\n';
    }
    t_os << "Variables:\n";
    for (const auto& var : t_model.vars()) {
        t_os << var << '\n';
    }
    return t_os;
}

#endif //OPTIMIZE_MODEL_H
