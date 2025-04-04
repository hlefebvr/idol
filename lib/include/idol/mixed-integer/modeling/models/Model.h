//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MODEL_H
#define IDOL_MODEL_H

#include <vector>
#include <string>
#include <functional>

#include "idol/general/utils/Vector.h"

#include "idol/mixed-integer/modeling/constraints/CtrVersion.h"
#include "idol/mixed-integer/modeling/variables/VarVersion.h"
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"
#include "idol/general/utils/Point.h"

#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/variables/Var.h"

#include "idol/general/optimizers/Optimizer.h"
#include "idol/general/optimizers/OptimizerFactory.h"

#include "idol/general/optimizers/Timer.h"
#include "idol/general/utils/LimitedWidthStream.h"
#include "idol/mixed-integer/modeling/constraints/QCtr.h"

namespace idol {
    static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

    class Env;
    class TempCtr;

    class Model;

}

/**
 * This class is used to represent a mathematical optimization model.
 */
class idol::Model {
public:
    enum Storage { ColumnOriented, RowOriented, Both };
private:
    Env& m_env;
    const unsigned int m_id;
    bool m_has_been_moved = false;

    ObjectiveSense m_sense = Minimize;

    QuadExpr<Var> m_objective;
    LinExpr<Ctr> m_rhs;
    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;
    std::vector<QCtr> m_qconstraints;

    Storage m_storage;
    mutable bool m_has_minor_representation = false;

    std::unique_ptr<Optimizer> m_optimizer;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    void throw_if_no_optimizer() const { if (!m_optimizer) { throw Exception("No optimizer was found."); } }

    Model(const Model& t_src);

    template<class T> void throw_if_unknown_object(const LinExpr<T>& t_expr);
    template<class T> void throw_if_unknown_object(const QuadExpr<T>& t_expr);
    void add_column_to_rows(const Var& t_var);
    void add_row_to_columns(const Ctr& t_ctr);
    void build_row(const Ctr& t_ctr);
    void build_column(const Var& t_var);
    void build_rows();
    void build_columns();

    bool column_storage_matters() const;
    bool row_storage_matters() const;
public:
    explicit Model(Env& t_env, Storage t_storage = Both);

    Model(Model&&) noexcept;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model();

    Var add_var(double t_lb, double t_ub, VarType t_type, double t_obj = 0., std::string t_name = "");

    Var add_var(double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr> t_column, std::string t_name = "");

    template<unsigned int N> Vector<Var, N> add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, double t_obj = 0., const std::string& t_name = "");

    void add(const Var& t_var);

    void add(const Var& t_var, TempVar t_temp_var);

    [[nodiscard]] bool has(const Var& t_var) const;

    void remove(const Var& t_var);

    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const { return m_variables; }

    Ctr add_ctr(TempCtr t_temp_ctr, std::string t_name = "");

    Ctr add_ctr(LinExpr<Var>&& t_lhs, CtrType t_type, double t_rhs, std::string t_name = "");

    template<unsigned int N> Vector<Ctr, N> add_ctrs(Dim<N> t_dim, CtrType t_type, double t_rhs, const std::string& t_name = "");

    void add(const Ctr& t_ctr);

    void add(const Ctr &t_ctr, TempCtr t_temp_ctr);

    [[nodiscard]] bool has(const Ctr& t_ctr) const;

    void remove(const Ctr& t_ctr);

    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const { return m_constraints; }

    QCtr add_qctr(TempQCtr t_temp_ctr, std::string t_name = "");

    QCtr add_qctr(QuadExpr<Var>&& t_expr, CtrType t_type, std::string t_name = "");

    template<unsigned int N> Vector<QCtr, N> add_qctrs(Dim<N> t_dim, CtrType t_type, const std::string& t_name = "");

    void add(const QCtr& t_ctr);

    void add(const QCtr &t_ctr, TempQCtr t_temp_ctr);

    [[nodiscard]] bool has(const QCtr& t_ctr) const;

    void remove(const QCtr& t_ctr);

    [[nodiscard]] ConstIteratorForward<std::vector<QCtr>> qctrs() const { return m_qconstraints; }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] Model* clone() const;

    [[nodiscard]] Model copy() const;

    void reserve_vars(unsigned int t_size);

    void reserve_ctrs(unsigned int t_size);

    void reserve_qctrs(unsigned int t_size);

    [[nodiscard]] Env& env() const { return const_cast<Model*>(this)->m_env; }

    void use(const OptimizerFactory& t_optimizer_factory);

    [[nodiscard]] bool has_optimizer() const;

    [[nodiscard]] bool has_optimizer_factory() const { return bool(m_optimizer_factory); }

    [[nodiscard]] const OptimizerFactory& optimizer_factory() const { return *m_optimizer_factory; }

    void unuse();

    template<class T, unsigned int N> void add_vector(const Vector<T, N>& t_vector);

    Optimizer& optimizer() { throw_if_no_optimizer(); return *m_optimizer; }

    [[nodiscard]] const Optimizer& optimizer() const { throw_if_no_optimizer(); return *m_optimizer; }

    void optimize();

    void write(const std::string& t_name);

    void update();

    [[nodiscard]] ObjectiveSense get_obj_sense() const;

    [[nodiscard]] const QuadExpr<Var>& get_obj_expr() const;

    [[nodiscard]] const LinExpr<Ctr>& get_rhs_expr() const;

    [[nodiscard]] double get_mat_coeff(const Ctr& t_ctr, const Var& t_var) const;

    [[nodiscard]] SolutionStatus get_status() const;

    [[nodiscard]] SolutionReason get_reason() const;

    [[nodiscard]] double get_best_obj() const;

    [[nodiscard]] double get_best_bound() const;

    void set_obj_sense(ObjectiveSense t_value);

    void set_obj_expr(const QuadExpr<Var>& t_objective);

    void set_obj_expr(QuadExpr<Var>&& t_objective);

    void set_rhs_expr(LinExpr<Ctr>&& t_rhs);

    void set_rhs_expr(const LinExpr<Ctr>& t_rhs);

    void set_obj_const(double t_constant);

    void set_mat_coeff(const Ctr& t_ctr, const Var& t_var, double t_coeff);

    [[nodiscard]] QCtr get_qctr_by_index(unsigned int t_index) const;

    [[nodiscard]] Ctr get_ctr_by_index(unsigned int t_index) const;

    [[nodiscard]] Var get_var_by_index(unsigned int t_index) const;

    [[nodiscard]] unsigned int get_ctr_index(const Ctr& t_ctr) const;

    [[nodiscard]] unsigned int get_qctr_index(const QCtr& tctr) const;

    [[nodiscard]] CtrType get_ctr_type(const Ctr& t_ctr) const;

    double get_ctr_rhs(const Ctr& t_ctr) const;

    [[nodiscard]] const LinExpr<Var>& get_ctr_row(const Ctr& t_ctr) const;

    const QuadExpr<Var>& get_qctr_expr(const QCtr& t_ctr) const;

    CtrType get_qctr_type(const QCtr& t_ctr) const;

    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const;

    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const;

    void set_ctr_rhs(const Ctr& t_ctr, double t_rhs);

    void set_ctr_type(const Ctr& t_ctr, CtrType t_type);

    void set_ctr_row(const Ctr& t_ctr, const LinExpr<Var>& t_row);

    void set_ctr_row(const Ctr& t_ctr, LinExpr<Var>&& t_row);

    [[nodiscard]] unsigned int get_var_index(const Var& t_var) const;

    [[nodiscard]] VarType get_var_type(const Var& t_var) const;

    [[nodiscard]] double get_var_lb(const Var& t_var) const;

    [[nodiscard]] double get_var_ub(const Var& t_var) const;

    [[nodiscard]] double get_var_primal(const Var& t_var) const;

    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const;

    [[nodiscard]] double get_var_ray(const Var& t_var) const;

    [[nodiscard]] const LinExpr<Ctr>& get_var_column(const Var& t_var) const;

    [[nodiscard]] double get_var_obj(const Var& t_var) const;

    void set_var_type(const Var& t_var, VarType t_type);

    void set_var_lb(const Var& t_var, double t_lb);

    void set_var_ub(const Var& t_var, double t_ub);

    void set_var_obj(const Var& t_var, double t_obj);

    void set_var_column(const Var& t_var, const LinExpr<Ctr>& t_column);

    void set_var_column(const Var& t_var, LinExpr<Ctr>&& t_column);

    [[nodiscard]] unsigned int get_n_solutions() const;

    [[nodiscard]] unsigned int get_solution_index() const;

    void set_solution_index(unsigned int t_index);

    void dump(std::ostream& t_os = std::cout) const;

    Storage storage() const { return m_storage; }

    void set_storage(Storage t_storage, bool t_reset_minor_representation = false);

    void reset_minor_representation();
};

template<class T, unsigned int N>
void idol::Model::add_vector(const Vector<T, N> &t_vector) {
    if constexpr (N == 1) {
        for (const auto& x : t_vector) {
            add(x);
        }
    } else  {
        for (const auto& x : t_vector) {
            add_vector<T, N - 1>(x);
        }
    }
}

template<unsigned int N>
idol::Vector<idol::Var, N> idol::Model::add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, double t_obj, const std::string& t_name) {
    auto result = Var::make_vector(m_env, t_dim, t_lb, t_ub, t_type, t_obj, t_name);
    add_vector<Var, N>(result);
    return result;
}

template<unsigned int N>
idol::Vector<idol::Ctr, N> idol::Model::add_ctrs(Dim<N> t_dim, CtrType t_type, double t_rhs, const std::string &t_name) {
    auto result = Ctr::make_vector(m_env, t_dim, t_type, t_rhs, t_name);
    add_vector<Ctr, N>(result);
    return result;
}

namespace idol {

    static auto save_primal(const Model &t_original_model, const Model &t_model) {

        PrimalPoint result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Optimal && status != Feasible && status != SubOptimal) {
            throw Exception("Primal values not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(t_model.get_best_obj());

        for (const auto &var: t_original_model.vars()) {
            const double value = t_model.get_var_primal(var);
            result.set(var, value);
        }

        return result;

    }

    static auto save_primal(const Model &t_original_model) {
        return save_primal(t_original_model, t_original_model);
    }

    static auto save_ray(const Model &t_original_model, const Model &t_model) {

        PrimalPoint result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Unbounded) {
            throw Exception("Ray not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(-Inf);

        for (const auto &var: t_original_model.vars()) {
            const double value = t_model.get_var_ray(var);
            result.set(var, value);
        }

        return result;

    }

    static auto save_ray(const Model &t_original_model) {
        return save_ray(t_original_model, t_original_model);
    }

    static auto save_dual(const Model &t_original_model, const Model &t_model) {

        DualPoint result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Optimal && status != Feasible) {
            throw Exception("Dual values not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(t_model.get_best_bound());

        for (const auto &ctr: t_original_model.ctrs()) {
            const double value = t_model.get_ctr_dual(ctr);
            result.set(ctr, value);
        }

        return result;

    }

    static auto save_dual(const Model &t_original_model) {
        return save_dual(t_original_model, t_original_model);
    }

    static auto save_farkas(const Model &t_original_model, const Model &t_model) {

        DualPoint result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Infeasible) {
            throw Exception("Farkas certificate not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(+Inf);

        for (const auto &ctr: t_original_model.ctrs()) {
            const double value = t_model.get_ctr_farkas(ctr);
            result.set(ctr, value);
        }

        return result;

    }

    static auto save_farkas(const Model &t_original_model) {
        return save_farkas(t_original_model, t_original_model);
    }

    static auto save_reduced_cost(const Model &t_original_model, const Model &t_model) {

        PrimalPoint result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Optimal && status != Feasible && status != SubOptimal) {
            throw Exception("Reduced costs not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(t_model.get_best_obj());

        for (const auto &var: t_original_model.vars()) {
            const double value = t_model.get_var_reduced_cost(var);
            result.set(var, value);
        }

        return result;

    }

    static auto save_reduced_cost(const Model &t_original_model) {
        return save_reduced_cost(t_original_model, t_original_model);
    }

}

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const idol::Model &t_model) {

        using namespace idol;

        if (t_model.storage() == Model::Storage::ColumnOriented) {
            std::cerr << "Warning: print a column-oriented model leads to the generation of a minor representation. "
                         "This operation can be slow and memory-consuming." << std::endl;
        }

        LimitedWidthStream stream(t_os, 120);

        if (t_model.get_obj_sense() == Minimize) {
            stream << "Minimize";
        } else {
            stream << "Maximize";
        }

        stream << std::endl << "\t" << t_model.get_obj_expr() << std::endl << "Subject To" << std::endl;

        for (const auto &ctr: t_model.ctrs()) {

            const auto linear = t_model.get_ctr_row(ctr);
            const double rhs = t_model.get_ctr_rhs(ctr);
            const auto type = t_model.get_ctr_type(ctr);

            stream << '\t' << ctr << ": ";

            stream << linear;

            switch (type) {
                case LessOrEqual:
                    stream << " <= ";
                    break;
                case Equal:
                    stream << " = ";
                    break;
                case GreaterOrEqual:
                    stream << " >= ";
                    break;
                default:
                    stream << " ?undefined? ";
            }

            stream << rhs << std::endl;
        }

        for (const auto& qctr : t_model.qctrs()) {
            const auto& expr = t_model.get_qctr_expr(qctr);
            const auto type = t_model.get_qctr_type(qctr);

            stream << '\t' << qctr << ": ";

            stream << expr;

            switch (type) {
                case LessOrEqual:
                    stream << " <= 0";
                    break;
                case Equal:
                    stream << " = 0";
                    break;
                case GreaterOrEqual:
                    stream << " >= 0";
                    break;
                default:
                    stream << " ?undefined? ";
            }

            stream << std::endl;
        }

        std::list<Var> generals, binaries;

        stream << "Bounds\n";
        for (const auto &var: t_model.vars()) {

            const double lb = t_model.get_var_lb(var);
            const double ub = t_model.get_var_ub(var);
            const int type = t_model.get_var_type(var);

            stream << '\t';

            if (!is_neg_inf(lb) && !is_pos_inf(ub)) {
                stream << lb << " <= " << var << " <= " << ub;
            } else if (!is_pos_inf(ub)) {
                stream << var << " <= " << ub;
            } else if (!is_neg_inf(lb)) {
                stream << var << " >= " << lb;
            } else {
                stream << var;
            }

            if (type == Binary) {
                binaries.emplace_back(var);
            } else if (type == Integer) {
                generals.emplace_back(var);
            }

            stream << std::endl;
        }

        if (!generals.empty()) {
            stream << "Generals\n";
            for (const auto& var : generals) {
                stream << '\t' << var.name() << std::endl;
            }
        }

        if (!binaries.empty()) {
            stream << "Binaries\n";
            for (const auto& var : binaries) {
                stream << '\t' << var.name() << std::endl;
            }
        }

        return t_os;
    }

}

#endif //IDOL_MODEL_H
