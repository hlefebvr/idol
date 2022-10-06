//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include "modeling/environment/ObjectManager.h"
#include "modeling/parameters/impl_Param.h"
#include "modeling/variables/impl_Variable.h"
#include "modeling/variables/TempVar.h"
#include "modeling/constraints/impl_Constraint.h"
#include "modeling/constraints/TempCtr.h"
#include "containers/IteratorForward.h"
#include "modeling/objective/Objective.h"
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
    std::vector<Param> m_parameters;
    std::vector<Var> m_variables;
    std::vector<Var> m_abstract_variables;
    std::vector<Ctr> m_constraints;

    template<class T> using iterator_forward = IteratorForward<std::vector<T>>;
    template<class T> using const_iterator_forward = ConstIteratorForward<std::vector<T>>;

    template<class T> void add_object(std::vector<T>& t_vec, const T& t_value);
    template<class T> void remove_object(std::vector<T>& t_vec, const T& t_value);
    template<class T> void free(std::vector<T>& t_vec);
    void add_column_to_rows(const Var& t_var);
    void add_row_to_columns(const Ctr& t_ctr);

    void add_created_variable(const Var& t_var);
public:
    explicit Model();

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model();

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] Objective objective() const { return Objective(m_variables, m_objective_offset); }

    iterator_forward<Param> parameters() { return iterator_forward<Param>(m_parameters); }
    [[nodiscard]] const_iterator_forward<Param> parameters() const { return const_iterator_forward<Param>(m_parameters); }

    iterator_forward<Var> variables() { return iterator_forward<Var>(m_variables); }
    [[nodiscard]] const_iterator_forward<Var> variables() const { return const_iterator_forward<Var>(m_variables); }

    iterator_forward<Ctr> constraints() { return iterator_forward<Ctr>(m_constraints); }
    [[nodiscard]] const_iterator_forward<Ctr> constraints() const { return const_iterator_forward<Ctr>(m_constraints); }

    Param add_parameter(const Var& t_variable, std::string t_name = "");
    void remove(const Param& t_param);

    Var add_variable(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    Var add_variable(double t_lb, double t_ub, VarType t_type, Constant t_objective_coefficient, std::string t_name = "");
    Var add_variable(TempVar t_temporary_variable, std::string t_name = "");
    void remove(const Var& t_var);

    Ctr add_constraint(CtrType t_type, Constant t_rhs, std::string t_name = "");
    Ctr add_constraint(TempCtr t_temporary_constraint, std::string t_name = "");
    void remove(const Ctr& t_ctr);

    void add_listener(Listener& t_listener) const;

    void update_objective(const Row& t_row);
    void update_objective_offset(Constant t_offset);
    void update_objective(const Var& t_var, Constant t_coefficient);
    void update_rhs(const Ctr& t_ctr, Constant t_coefficient);
    void update_coefficient(const Ctr& t_ctr, const Var& t_var, Constant t_coefficient);
    void update_lb(const Var& t_var, double t_lb);
    void update_ub(const Var& t_var, double t_ub);
    void update_type(const Var& t_var, VarType t_type);
    void update_type(const Ctr& t_ctr, CtrType t_type);

    class Transform;

    Map<Ctr, Expr<Var>> move_to(Model& t_destination, const std::function<bool(const Ctr&)>& t_indicator);
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
    m_objects.free(t_value);
    t_vec.pop_back();
}

template<class T>
void Model::free(std::vector<T> &t_vec) {
    for (auto obj : t_vec) {
        m_objects.template free(obj);
    }
}

static std::ostream& operator<<(std::ostream& t_os, const Model& t_model) {
    t_os << "Minimize " << t_model.objective() << "\nSubject to:\n";
    for (const auto& ctr : t_model.constraints()) {
        t_os << ctr << '\n';
    }
    t_os << "Variables:\n";
    for (const auto& var : t_model.variables()) {
        t_os << var << '\n';
    }
    return t_os;
}

/**
 * Model transformation object.
 *
 * This class is used to provide more advanced model modifications than the "update" functions present in
 * Model. In particular, it provides tools to move modeling objects from one Model to another or to make
 * changes to the underlying matrix.
 *
 * Note that no public method leaves a Model in an undesirable state.
 */
class Model::Transform {
    Model& m_model;

    template<class T> void swap(std::vector<T>& t_container, const T& t_a, const T& t_b);

    template<class T> void hard_move(std::vector<T>& t_source, std::vector<T>& t_dest, unsigned int t_dest_id, const T& t_object);
    void hard_move(Model& t_destination, const Ctr& t_ctr);
    void hard_move(Model& t_destination, const Var& t_var);
public:
    explicit Transform(Model& t_model);

    /**
     * Swaps two columns in the underlying matrix
     */
    void swap(const Var& t_a, const Var& t_b);

    /**
     * Swaps two rows in the underlying matrix
     */
    void swap(const Ctr& t_a, const Ctr& t_b);

    /**
     * Swaps two parameters in the underlying matrix
     */
    void swap(const Param& t_a, const Param& t_b);

    /**
     * Moves a set of constraints from the source model to the destination model given as parameter.
     * The moved constraints are those for which the function given as parameter returns true.
     *
     * Note that every variable which appear in the indicated constraints are also moved to the destination
     * model. Remaining illegal terms are removed from the original model and a map of such removals is returned
     * by the method (e.g., consider moving only constraint `c2: x + y <= 1` while `y` also appears in constraint `c1: 2 y + z >= 1`.
     * Then `y` is removed from the original model (since `y` does no longer belong to the original model)
     * and an entry `(c1, 2 y)` is returned by the method.).
     * @param t_destination The destination model.
     * @param t_indicator The indicator function for constraints to be moved.
     * @return The illegal terms removed from the original model.
     */
    Map<Ctr, Expr<Var>> move(Model& t_destination, const std::function<bool(const Ctr &)> &t_indicator);
};

template<class T>
void Model::Transform::swap(std::vector<T> &t_container, const T &t_a, const T &t_b) {

    t_container[t_a.index()] = t_b;
    t_container[t_b.index()] = t_a;

    const unsigned int a_index = t_a.index();
    m_model.m_objects.impl(t_a).set_index(t_b.index());
    m_model.m_objects.impl(t_b).set_index(a_index);

}

template<class T>
void Model::Transform::hard_move(std::vector<T> &t_source, std::vector<T> &t_dest, unsigned int t_dest_id, const T& t_object) {

    swap(t_object, t_source.back());
    t_source.pop_back();

    m_model.m_objects.set_model_id(t_object, t_dest_id);
    m_model.m_objects.impl(t_object).set_index(t_dest.size());
    t_dest.emplace_back(t_object);

}

#endif //OPTIMIZE_MODEL_H
