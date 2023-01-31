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

#include "backends/Backend.h"

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

    // Backend
    std::unique_ptr<Backend> m_backend;
    void throw_if_no_backend() const;

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
    void add(const Var& t_var, double t_lb, double t_ub, int t_type, Column&& t_column);
    void add(const Var& t_var, double t_lb, double t_ub, int t_type, const Column& t_column);
    void add(const Var& t_var, double t_lb, double t_ub, int t_type);
    void add(const Var& t_var);
    void remove(const Var& t_var);
    [[nodiscard]] auto vars() const { return ConstIteratorForward<std::vector<Var>>(m_variables); }

    // Constraints
    void add(const Ctr& t_ctr);
    void add(const Ctr& t_ctr, TempCtr&& t_temp_ctr);
    void add(const Ctr& t_ctr, const TempCtr& t_temp_ctr);
    void remove(const Ctr& t_ctr);
    [[nodiscard]] auto ctrs() const { return ConstIteratorForward<std::vector<Ctr>>(m_constraints); }

    template<class T, unsigned int N> void add(const Vector<T, N>& t_vector);

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
    [[nodiscard]] const Constant& get(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr) const override;
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

    // Backend
    template<class T, class ...ArgsT> T& set_backend(ArgsT&& ...t_args);
    void reset_backend() { m_backend.reset(); }
    bool has_backend() { return (bool) m_backend; }
    void optimize();
    void update();
    void write(const std::string& t_name);
};

template<class T, unsigned int N>
void Model::add(const Vector<T, N>& t_vector) {
    if constexpr (N == 1) {
        for (const auto& x : t_vector) {
            add(x);
        }
    } else  {
        for (const auto& x : t_vector) {
            add<T, N - 1>(x);
        }
    }
}

template<class T, class... ArgsT>
T &Model::set_backend(ArgsT &&... t_args) {
    static_assert(std::is_base_of_v<Backend, T>);
    if (m_backend) {
        throw Exception("Backend already exists.");
    }
    auto* result = new T(*this, std::forward<ArgsT>(t_args)...);
    m_backend.reset(result);
    m_backend->initialize();
    return *result;
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
        const int type = t_model.get(Attr::Var::Type, var);

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
