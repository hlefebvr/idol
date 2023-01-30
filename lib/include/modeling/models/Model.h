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

#include "../attributes/AttributeManagers_Base.h"
#include "Attributes_Model.h"
#include "../constraints/Attributes_Ctr.h"
#include "../variables/Attributes_Var.h"

class Env;
class Column;
class TempCtr;

/**
 * This class is used to represent a mathematical optimization model.
 */
class Model : public Matrix, public AttributeManagers::Base {
    Env& m_env;
    const unsigned int m_id;

    int m_sense = Minimize;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;

    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    // Matrix hooks
    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;

    template<class T, int N, int I = 0> Vector<T, N-I> create_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one);
public:
    explicit Model(Env& t_env);

    Model(const Model&) = delete;
    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model() override;

    // Variables
    Var create_var(double t_lb, double t_ub, int t_type, Column&& t_column, std::string t_name = "");
    Var create_var(double t_lb, double t_ub, int t_type, const Column& t_column, std::string t_name = "");
    Var create_var(double t_lb, double t_ub, int t_type, std::string t_name = "");
    template<int N> Vector<Var, N> create_vars(const Dim<N>& t_dim, double t_lb, double t_ub, int t_type, const std::string& t_name = "");

    void add_var(const Var& t_var, double t_lb, double t_ub, int t_type, Column&& t_column);
    void add_var(const Var& t_var, double t_lb, double t_ub, int t_type, const Column& t_column);
    void add_var(const Var& t_var, double t_lb, double t_ub, int t_type);
    //template<int N> void add_vars(const Vector<MyVar, N>& t_vars, double t_lb, double t_ub, int t_type);

    void remove(const Var& t_var);

    [[nodiscard]] auto vars() const { return ConstIteratorForward<std::vector<Var>>(m_variables); }

    // Constraints
    Ctr create_ctr(TempCtr&& t_temp_ctr, std::string t_name = "");
    Ctr create_ctr(const TempCtr& t_temp_ctr, std::string t_name = "");
    Ctr create_ctr(int t_type, std::string t_name = "");

    void add_ctr(const Ctr& t_ctr, TempCtr&& t_temp_ctr);
    void add_ctr(const Ctr& t_ctr, const TempCtr& t_temp_ctr);
    void add_ctr(const Ctr& t_ctr, int t_type);

    void remove(const Ctr& t_ctr);

    [[nodiscard]] auto ctrs() const { return ConstIteratorForward<std::vector<Ctr>>(m_constraints); }

    // Model
    [[nodiscard]] unsigned int id() const { return m_id; }

    using AttributeManagers::Base::set;
    using AttributeManagers::Base::get;

    // Models' attributes
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, void> &t_attr) const override;
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, void>& t_attr) const override;
    [[nodiscard]] const Expr<Var, Var>& get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr) const override;
    [[nodiscard]] const LinExpr<Ctr>& get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr) const override;
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const override;
    void set(const AttributeWithTypeAndArguments<int, void> &t_attr, int t_value) override;
    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override;
    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, Constant &&t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override;

    // Constraints' attributes
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Ctr>& t_attr, const Ctr& t_ctr) const override;
    [[nodiscard]] const Row& get(const AttributeWithTypeAndArguments<Row, Ctr>& t_attr, const Ctr& t_ctr) const override;
    void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    void set(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
    void set(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override;

    // Variables' attributes
    [[nodiscard]] int get(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_ctr) const override;
    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] const Column& get(const AttributeWithTypeAndArguments<Column, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Var>& t_attr, const Var& t_var) const override;
    void set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) override;
    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override;
    void set(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override;
};

template<class T, int N, int I>
Vector<T, N - I> Model::create_many(const Dim<N> &t_dims, const std::string &t_name,
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
Vector<Var, N> Model::create_vars(const Dim<N> &t_dim, double t_lb, double t_ub, int t_type, const std::string& t_name) {
    return create_many<Var, N>(t_dim, t_name.empty() ? "Var" : t_name, [&](const std::string& t_name){
        return create_var(t_lb, t_ub, t_type, t_name);
    });
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

        if (!is_neg_inf(lb) && !is_pos_inf(ub)) {
            t_os << lb << " <= " << var << " <= " << ub << '\n';
        } else if (!is_pos_inf(ub)) {
            t_os << var << " <= " << ub << '\n';
        } else if (!is_neg_inf(lb)) {
            t_os << var << " >= " << lb << '\n';
        } else {
            t_os << var << '\n';
        }
    }
    return t_os;
}


#endif //IDOL_MODEL_H
