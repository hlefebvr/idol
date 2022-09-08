//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include "../Types.h"
#include "modeling/environment/ObjectManager.h"
#include "modeling/parameters/Param.h"
#include "modeling/parameters/impl_Param.h"
#include "modeling/variables/Variable.h"
#include "modeling/variables/impl_Variable.h"
#include "modeling/constraints/Constraint.h"
#include "modeling/constraints/impl_Constraint.h"
#include "modeling/constraints/TempCtr.h"
#include <string>
#include <vector>

class Env;
class Column;

class Model {
    ObjectManager m_objects;

    std::vector<Param> m_parameters;
    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    template<class T> void add_object(std::vector<T>& t_vec, const T& t_value);
    template<class T> void remove_object(std::vector<T>& t_vec, const T& t_value);
    void add_column_to_rows(const Var& t_var);
    void add_row_to_columns(const Ctr& t_ctr);
public:
    explicit Model(Env& t_env);

    Param add_parameter(double t_lb, double t_ub, VarType t_type, std::string t_name);
    void remove(const Param& t_param);

    Var add_variable(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");
    Var add_variable(double t_lb, double t_ub, VarType t_type, Coefficient t_objective_coefficient, std::string t_name = "");
    void remove(const Var& t_var);

    Ctr add_constraint(CtrType t_type, Coefficient t_rhs, std::string t_name = "");
    Ctr add_constraint(TempCtr t_temporary_constraint, std::string t_name = "");
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
    const auto index = m_objects.impl(t_value).index();
    t_vec[index] = t_vec.back();
    m_objects.impl(t_vec[index]).set_index(index);
    m_objects.free(t_value);
    t_vec.pop_back();
}


#endif //OPTIMIZE_MODEL_H
