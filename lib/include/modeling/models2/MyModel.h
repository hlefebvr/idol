//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYMODEL_H
#define IDOL_MYMODEL_H

#include <vector>
#include <string>
#include <functional>
#include "../../containers/IteratorForward.h"
#include "MyVar.h"
#include "MyCtr.h"
#include "containers/Vector.h"

class MyEnv;
class Column;
class TempCtr;

/**
 * This class is used to represent an mathematical optimization model.
 */
class MyModel {
    MyEnv& m_env;
    const unsigned int m_id;

    std::vector<MyVar> m_variables;
    std::vector<MyCtr> m_constraints;

    template<class T, int N, int I = 0> Vector<T, N-I> create_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one);
public:
    explicit MyModel(MyEnv& t_env);

    MyModel(const MyModel&) = delete;
    MyModel(MyModel&&) noexcept = default;

    MyModel& operator=(const MyModel&) = delete;
    MyModel& operator=(MyModel&&) noexcept = delete;

    ~MyModel();

    // Variables
    MyVar create_var(double t_lb, double t_ub, int t_type, Column&& t_column, std::string t_name = "");
    MyVar create_var(double t_lb, double t_ub, int t_type, const Column& t_column, std::string t_name = "");
    MyVar create_var(double t_lb, double t_ub, int t_type, std::string t_name = "");
    template<int N> Vector<MyVar, N> create_vars(const Dim<N>& t_dim, double t_lb, double t_ub, int t_type, const std::string& t_name = "");

    void add_var(const MyVar& t_var, double t_lb, double t_ub, int t_type, Column&& t_column);
    void add_var(const MyVar& t_var, double t_lb, double t_ub, int t_type, const Column& t_column);
    void add_var(const MyVar& t_var, double t_lb, double t_ub, int t_type);
    //template<int N> void add_vars(const Vector<MyVar, N>& t_vars, double t_lb, double t_ub, int t_type);

    void remove(const MyVar& t_var);

    [[nodiscard]] auto vars() const { return ConstIteratorForward<std::vector<MyVar>>(m_variables); }

    // Constraints
    MyCtr create_ctr(TempCtr&& t_temp_ctr, std::string t_name = "");
    MyCtr create_ctr(const TempCtr& t_temp_ctr, std::string t_name = "");
    MyCtr create_ctr(int t_type, std::string t_name = "");

    void add_ctr(const MyCtr& t_ctr, TempCtr&& t_temp_ctr);
    void add_ctr(const MyCtr& t_ctr, const TempCtr& t_temp_ctr);
    void add_ctr(const MyCtr& t_ctr, int t_type);

    void remove(const MyCtr& t_var);

    [[nodiscard]] auto ctrs() const { return ConstIteratorForward<std::vector<MyCtr>>(m_constraints); }

    //

    [[nodiscard]] const VarVersion& get(const MyVar& t_var) const;

    [[nodiscard]] unsigned int id() const { return m_id; }
};

template<class T, int N, int I>
Vector<T, N - I> MyModel::create_many(const Dim<N> &t_dims, const std::string &t_name,
                                      const std::function<T(const std::string &)> &t_add_one) {
    Vector<T, N - I> result;
    const unsigned int size = t_dims[I];
    result.reserve(size);
    for (unsigned int i = 0 ; i < size ; ++i) {
        const std::string name = t_name + "_" + std::to_string(i);
        if constexpr (I == N - 1) {
            result.emplace_back( t_add_one(name) );
        } else {
            result.emplace_back( create_many<T, N, I+1>(t_dims, name, t_add_one) );
        }
    }
    return result;
}

template<int N>
Vector<MyVar, N> MyModel::create_vars(const Dim<N> &t_dim, double t_lb, double t_ub, int t_type, const std::string& t_name) {
    return create_many<MyVar, N>(t_dim, t_name.empty() ? "Var" : t_name, [&](const std::string& t_name){
        return create_var(t_lb, t_ub, t_type, t_name);
    });
}

#endif //IDOL_MYMODEL_H
