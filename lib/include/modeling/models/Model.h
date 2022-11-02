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
class Model {
    static unsigned int s_id;

    const unsigned int m_id = s_id++;

    ObjectManager m_objects;
    mutable ListenerManager m_listeners;

    Constant m_objective_offset;

    ObjSense m_objective_sense;
    Expr<Var> m_objective;
    Expr<Ctr> m_rhs;
    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    template<class T> using iterator_forward = IteratorForward<std::vector<T>>;
    template<class T> using const_iterator_forward = ConstIteratorForward<std::vector<T>>;

    template<class T, int N, int I = 0> Vector<T, N-I> add_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one);
    template<class T> void add_object(std::vector<T>& t_vec, const T& t_value);
    template<class T> void remove_object(std::vector<T>& t_vec, const T& t_value);
    void add_column_to_rows(const Var& t_var);
    void add_row_to_columns(const Ctr& t_ctr);

    void add_created_variable(const Var& t_var);
public:
    explicit Model(ObjSense t_sense = Minimize);

    ~Model();

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    /* Gets */

    [[nodiscard]] unsigned int id() const { return m_id; }

    ObjSense sense() const { return m_objective_sense; }

    const Expr<Var>& obj() const { return m_objective; }
    //[[nodiscard]] Objective objective() const { return Objective(m_variables, m_objective_offset); }

    const Expr<Ctr>& rhs() const { return m_rhs; }

    iterator_forward<Var> variables() { return iterator_forward<Var>(m_variables); }
    [[nodiscard]] const_iterator_forward<Var> variables() const { return const_iterator_forward<Var>(m_variables); }

    iterator_forward<Ctr> constraints() { return iterator_forward<Ctr>(m_constraints); }
    [[nodiscard]] const_iterator_forward<Ctr> constraints() const { return const_iterator_forward<Ctr>(m_constraints); }

    /* Adds */
    Var add_variable(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    Var add_variable(double t_lb, double t_ub, VarType t_type, Constant t_objective_coefficient, std::string t_name = "");
    Var add_variable(TempVar t_temporary_variable, std::string t_name = "");
    template<int N> Vector<Var, N> add_variables(const Dim<N>& t_dim, double t_lb, double t_ub, VarType t_type, const Constant& t_objective_coefficient, const std::string& t_name = "");
    template<int N> Vector<Var, N> add_variables(const Dim<N>& t_dim, const TempVar& t_temporary_variable, const std::string& t_name = "");

    Ctr add_constraint(CtrType t_type, Constant t_rhs, std::string t_name = "");
    Ctr add_constraint(TempCtr t_temporary_constraint, std::string t_name = "");
    template<int N> Vector<Ctr, N> add_constraints(const Dim<N>& t_dim, CtrType t_type, const Constant& t_rhs, const std::string& t_name = "");
    template<int N> Vector<Ctr, N> add_constraints(const Dim<N>& t_dim, const TempCtr& t_temporary_constraint, const std::string& t_name = "");

    void add_listener(Listener& t_listener) const;

    /* Updates */
    //void update_obj(const Row& t_row);
    void update_obj(Expr<Var>&& t_obj);
    void update_obj(const Expr<Var>& t_obj);
    void update_obj_sense(ObjSense t_sense);
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
};

template<class T>
void Model::add_object(std::vector<T> &t_vec, const T &t_value) {
    const unsigned int index = t_vec.size();
    t_vec.template emplace_back(t_value);
    m_objects.impl(t_value).set_index(index);
}

template<class T>
void Model::remove_object(std::vector<T> &t_vec, const T &t_value) {
    const auto index = t_value.index();
    t_vec[index] = t_vec.back();
    m_objects.impl(t_vec[index]).set_index(index);
    m_objects.impl(t_value).set_status(Removed);
    m_objects.template free(t_value);
    t_vec.pop_back();
}

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
Vector<Var, N> Model::add_variables(const Dim<N> &t_dim, double t_lb, double t_ub, VarType t_type,
                                    const Constant &t_objective_coefficient, const std::string& t_name) {
    return add_many<Var, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_variable(t_lb, t_ub, t_type, t_objective_coefficient, t_name);
    });
}

template<int N>
Vector<Var, N> Model::add_variables(const Dim<N> &t_dim, const TempVar& t_temporary_variable, const std::string &t_name) {
    return add_many<Var, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_variable(TempVar(t_temporary_variable), t_name);
    });
}

template<int N>
Vector<Ctr, N> Model::add_constraints(const Dim<N> &t_dim, CtrType t_type, const Constant& t_rhs, const std::string &t_name) {
    return add_many<Ctr, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_constraint(t_type, t_rhs, t_name);
    });
}

template<int N>
Vector<Ctr, N>
Model::add_constraints(const Dim<N> &t_dim, const TempCtr &t_temporary_constraint, const std::string &t_name) {
    return add_many<Ctr, N>(t_dim, t_name, [&](const std::string& t_name){
        return add_constraint(t_temporary_constraint, t_name);
    });
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {
    t_os << t_model.sense() << " " << t_model.obj() << "\nSubject to:\n";
    for (const auto& ctr : t_model.constraints()) {
        t_os << ctr << '\n';
    }
    t_os << "Variables:\n";
    for (const auto& var : t_model.variables()) {
        t_os << var << '\n';
    }
    return t_os;
}

#endif //OPTIMIZE_MODEL_H
