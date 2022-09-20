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

class Model {
    static unsigned int s_id;

    const unsigned int m_id = s_id++;

    ObjectManager m_objects;
    mutable ListenerManager m_listeners;

    Coefficient m_objective_offset;
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
    explicit Model(Env& t_env);

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
    Var add_variable(double t_lb, double t_ub, VarType t_type, Coefficient t_objective_coefficient, std::string t_name = "");
    Var add_variable(TempVar t_temporary_variable, std::string t_name = "");
    void remove(const Var& t_var);

    Var add_virtual_variable(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    Var add_virtual_variable(double t_lb, double t_ub, VarType t_type, Coefficient t_objective_coefficient, std::string t_name = "");
    Var add_virtual_variable(TempVar t_temporary_variable, std::string t_name = "");

    Ctr add_constraint(CtrType t_type, Coefficient t_rhs, std::string t_name = "");
    Ctr add_constraint(TempCtr t_temporary_constraint, std::string t_name = "");
    void remove(const Ctr& t_ctr);

    void add_listener(Listener& t_listener) const;

    void update_objective(const Row& t_row);
    void update_objective_offset(Coefficient t_offset);
    void update_objective(const Var& t_var, Coefficient t_coefficient);
    void update_rhs(const Ctr& t_ctr, Coefficient t_coefficient);
    void update_coefficient(const Ctr& t_ctr, const Var& t_var, Coefficient t_coefficient);
    void update_lb(const Var& t_var, double t_lb);
    void update_ub(const Var& t_var, double t_ub);
    void update_type(const Var& t_var, VarType t_type);
    void update_type(const Ctr& t_ctr, CtrType t_type);
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

#endif //OPTIMIZE_MODEL_H
