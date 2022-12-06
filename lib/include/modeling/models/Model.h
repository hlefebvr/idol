//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MODEL_H
#define OPTIMIZE_MODEL_H

#include "../variables/TempVar.h"
#include "../constraints/TempCtr.h"
#include "../expressions/Expr.h"
#include "../../containers/IteratorForward.h"
#include "../../containers/Vector.h"
#include "../variables/VarAttributes.h"
#include "../objects/ObjectStore.h"
#include "../constraints/CtrAttributes.h"
#include "../matrix/Matrix.h"
#include "AttributeManagers_Base.h"
#include "../constraints/Attributes_Ctr.h"
#include "../variables/Attributes_Var.h"
#include "Attributes_Model.h"
#include <vector>

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
class Model : public Matrix, public AttributeManagers::Base {

    int m_objective_sense;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;
    ObjectStore<Var, VarAttributes> m_variables;
    ObjectStore<Ctr, CtrAttributes> m_constraints;

    template<class T, int N, int I = 0> Vector<T, N-I> add_many(const Dim<N>& t_dims, const std::string& t_name, const std::function<T(const std::string& t_name)>& t_add_one);

    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;

protected:
    Model(const Model&) = default;
public:
    explicit Model(Sense t_sense = Minimize);

    Model(Model&&) noexcept = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    /* Iterators */
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

    /* Removes */
    void remove(const Var& t_var);
    void remove(const Ctr& t_ctr);

    Model clone() const;

    using AttributeManagers::Base::set;
    using AttributeManagers::Base::get;

    /// Set
    // Var
    void set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) override;
    void set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override;

    // Ctr
    void set(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) override;

    // Model
    void set(const AttributeWithTypeAndArguments<int, void> &t_attr, int t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var, Constant &&t_value) override;
    void set(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override;
    void set(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr, LinExpr<Ctr> &&t_value) override;
    void set(const AttributeWithTypeAndArguments<Constant, void> &t_attr, Constant &&t_value) override;

    /// Get
    // Var
    double get(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var) const override;
    const Column &get(const AttributeWithTypeAndArguments<Column, Var> &t_attr, const Var &t_var) const override;
    const Constant &get(const AttributeWithTypeAndArguments<Constant, Var> &t_attr, const Var &t_var) const override;
    int get(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var) const override;

    // Ctr
    int get(const AttributeWithTypeAndArguments<int, Ctr> &t_attr, const Ctr &t_ctr) const override;
    const Row &get(const AttributeWithTypeAndArguments<Row, Ctr> &t_attr, const Ctr &t_ctr) const override;
    const Constant &get(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr) const override;

    // Model
    const Constant & get(const AttributeWithTypeAndArguments<Constant, Ctr, Var> &t_attr, const Ctr &t_ctr, const Var &t_var) const override;
    const Expr<Var, Var> &get(const AttributeWithTypeAndArguments<Expr<Var, Var>, void> &t_attr) const override;
    const LinExpr<Ctr> &get(const AttributeWithTypeAndArguments<LinExpr<Ctr>, void> &t_attr) const override;
    const Constant &get(const AttributeWithTypeAndArguments<Constant, void> &t_attr) const override;
    int get(const AttributeWithTypeAndArguments<int, void> &t_attr) const override;

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

#endif //OPTIMIZE_MODEL_H
