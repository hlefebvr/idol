//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYMODEL_H
#define IDOL_MYMODEL_H

#include <vector>
#include <string>
#include "MyVar.h"
#include "MyCtr.h"

class MyEnv;

/**
 * This class is used to represent an mathematical optimization model.
 */
class MyModel {
    MyEnv& m_env;
    const unsigned int m_id;

    std::vector<MyVar> m_variables;
    //std::vector<MyCtr> m_constraints;
public:
    explicit MyModel(MyEnv& t_env);

    ~MyModel();

    MyVar create_var(double t_lb, double t_ub, std::string t_name = "");

    void add_var(const MyVar& t_var, double t_lb, double t_ub);

    void remove(const MyVar& t_var);

    [[nodiscard]] const VarVersion& get(const MyVar& t_var) const;

    [[nodiscard]] unsigned int id() const { return m_id; }
};

#endif //IDOL_MYMODEL_H
