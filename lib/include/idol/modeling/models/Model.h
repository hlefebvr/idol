//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MODEL_H
#define IDOL_MODEL_H

#include <vector>
#include <string>
#include <functional>

#include "idol/containers/Vector.h"

#include "idol/modeling/matrix/Matrix.h"
#include "idol/modeling/constraints/CtrVersion.h"
#include "idol/modeling/variables/VarVersion.h"
#include "idol/modeling/expressions/Expr.h"
#include "idol/modeling/solutions/Solution.h"

#include "Model.h"

#include "idol/optimizers/Optimizer.h"
#include "idol/optimizers/OptimizerFactory.h"

#include "idol/optimizers/Timer.h"
#include "idol/containers/LimitedWidthStream.h"

namespace idol {
    static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

    class Env;
    class Column;
    class TempCtr;

    class Model;

}

/**
 * This class is used to represent a mathematical optimization model.
 */
class idol::Model : public Matrix {
    Env& m_env;
    const unsigned int m_id;
    bool m_has_been_moved = false;

    ObjectiveSense m_sense = Minimize;
    Expr<Var> m_objective;
    LinExpr<Ctr> m_rhs;

    std::vector<Var> m_variables;
    std::vector<Ctr> m_constraints;

    std::unique_ptr<Optimizer> m_optimizer;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    void throw_if_no_optimizer() const { if (!m_optimizer) { throw Exception("No optimizer was found."); } }

    // Matrix hooks
    Expr<Var> &access_obj() override;
    LinExpr<Ctr> &access_rhs() override;
    Column &access_column(const Var &t_var) override;
    Row &access_row(const Ctr &t_ctr) override;

    Model(const Model& t_src);
public:
    /**
     * Creates a new model for a mathematical optimization problem.
     *
     * Example:
     * ```cpp
     * Env env;
     * Model model(env);
     * ```
     * @param t_env the optimization environment which will store the model
     */
    explicit Model(Env& t_env);

    Model(Env& t_env, ObjectiveSense t_sense);

    Model(Model&&) noexcept;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = delete;

    ~Model() override;

    /**
     * Creates a new decision variable in the model and returns it.
     *
     * Example:
     * ```cpp
     * model.add_var(0, 1, Binary, "x");
     * ```
     * @param t_lb the lower bound in the model
     * @param t_ub the upper bound in the model
     * @param t_type the type in the model
     * @param t_name the name of the variable
     * @return the created variable
     */
    Var add_var(double t_lb, double t_ub, VarType t_type, std::string t_name = "");

    /**
     * Creates a new decision variable in the model and returns it.
     *
     * Example:
     * ```cpp
     * model.add_var(0, 1, Binary, Column(-2), "x"); // Adds an objective coefficient of -2
     * ```
     *
     * Attention: every constraint involved in t_column must already be part of the model.
     * @param t_lb the lower bound in the model
     * @param t_ub the upper bound in the model
     * @param t_type the type in the model
     * @param t_column the column in the model
     * @param t_name the name of the variable
     * @return the created variable
     */
    Var add_var(double t_lb, double t_ub, VarType t_type, Column t_column, std::string t_name = "");

    /**
     * Creates multiple decision variables in the model and returns them.
     *
     * Example:
     * ```cpp
     * auto x = model.add_vars(Dim<2>(n, m), 0, 1, Binary, "x");
     *
     * for (unsigned int i = 0 ; i < n ; ++i) {
     *      for (unsigned int j = 0 ; j < m ; ++j) {
     *          std::cout << x[i][j].name() << std::endl;
     *      }
     * }
     * ```
     *
     * @tparam N the number of dimensions for the indices (e.g., N = 2 will create variables with 2 indices, \f$ x_{ij} \f$)
     * @param t_dim the dimensions for the indices
     * @param t_lb the lower bound for the variables in the model
     * @param t_ub the upper bound for the variables in the model
     * @param t_type the type for the variables in the model
     * @param t_name the base name for the variables (variables are then named by a combination of this name and indices)
     * @return the create variables
     */
    template<unsigned int N> Vector<Var, N> add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, const std::string& t_name = "");

    /**
     * Adds an existing variable to the model.
     *
     * Example:
     * ``` cpp
     * Env env;
     *
     * Model model1(env);
     * auto x = model1.add_var(0, Inf, Continuous, "x");
     *
     * Model model2(env);
     * model2.add(x);
     * ```
     *
     * Note that the default version of the variable is added to the model (i.e., the variable is added identical to its
     * first version as defined at creation time).
     * @param t_var the variable to add
     */
    void add(const Var& t_var);

    /**
     * Adds an existing variable to the model with different attributes than the default version (i.e., the variable is
     * added to the model and has the attributes defined in t_temp_var).
     *
     * Example:
     * ``` cpp
     * Env env;
     *
     * Model model1(env);
     * auto x = model1.add_var(0, Inf, Continuous, Column(-2), "x"); // x has an objective cost of -2 in model1
     *
     * Model model2(env);
     * model2.add(x, TempVar(0, Inf, Continuous, Column(0))); // x has an objective cost of 0 in model2
     * ```
     *
     * Attention: every constraint involved in t_temp_var must already be part of the model.
     * @param t_var the variable to add
     * @param t_temp_var the attributes of the variable in the model
     */
    void add(const Var& t_var, TempVar t_temp_var);

    /**
     * Returns true if and only if the variable is part of the model
     *
     * Example:
     * ```cpp
     * auto x = model.add_var(-Inf, Inf, Continuous, "x");
     *
     * std::cout << model.has(x) << std::endl; // output: 1
     * ```
     * @param t_var the variable
     * @return true if and only if the variable is part of the model
     */
    [[nodiscard]] bool has(const Var& t_var) const;

    /**
     * Removes a variable from the model.
     *
     * Example:
     * ```cpp
     * auto x = model.add_var(0, 1, Binary, "x");
     *
     * std::cout << model.has(x) << std::endl; // output: 1
     *
     * model.remove(x);
     *
     * std::cout << model.has(x) << std::endl; // output: 0
     * ```
     *
     * This also removes every reference to the variable in the objective and within the constraints.
     * @param t_var
     */
    void remove(const Var& t_var);

    /**
     * Returns an object able to construct iterators over the set of variables of the model.
     *
     * Example:
     *
     * ```cpp
     * for (const Var& var : model.vars()) {
     *      std::cout << var.name() << std::endl;
     * }
     * ```
     * @return an object able to construct iterators over the set of variables of the model.
     */
    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const { return m_variables; }

    /**
     * Creates a new constraint in the model and returns it.
     *
     * Example:
     * ```cpp
     * auto constraint = model.add_ctr(x[0] + 2 * x[1] <= 1.5, "constraint");
     * ```
     *
     * Attention: every variable involved in t_temp_ctr must already be part of the model.
     * @param t_temp_ctr the constraint attributes in the model
     * @param t_name the name of the constraint
     * @return
     */
    Ctr add_ctr(TempCtr t_temp_ctr, std::string t_name = "");

    /**
     * Creates a new constraint in the model and returns it.
     *
     * Example:
     * ```cpp
     * auto constraint = model.add_ctr(Row(x[0] + 2 * x[1], 1.5), LessOrEqual);
     * ```
     * @param t_row the row of the constraint in the model
     * @param t_type the type of the constraint in the model
     * @param t_name the name of the constraint
     * @return the created constraint
     */
    Ctr add_ctr(Row&& t_row, CtrType t_type, std::string t_name = "");

    /**
     * Creates multiple constraints in the model and returns them.
     *
     * Example:
     * ```cpp
     * auto constraints = model.add_ctrs(Dim<2>(m, n), EqualTo, 1, "constraint");
     *
     * for (unsigned int i = 0 ; i < n ; ++i) {
     *      for (unsigned int j = 0 ; j < m ; ++j) {
     *          std::cout << constraints[i][j].name() << std::endl;
     *      }
     * }
     * ```
     * @tparam N the number of dimensions for the indices
     * @param t_dim the dimensions for the indices
     * @param t_type the type of the constraints in the model
     * @param t_constant the right hand-side of the constraints in the model
     * @param t_name the base name for the constraints (constraints are then named by a combination of this name and indices)
     * @return the created constraints
     */
    template<unsigned int N> Vector<Ctr, N> add_ctrs(Dim<N> t_dim, CtrType t_type, const Constant& t_constant, const std::string& t_name = "");

    /**
     * Adds an existing variable to the model.
     *
     * Example:
     * ```cpp
     * Env env;
     *
     * Model model1(env);
     * auto x = model.add_vars(Dim<1>(2), 0, Inf, Continuous, "x");
     * auto constraint = model1.add_ctr(x[0] + 2 * x[1] <= 1.5);
     *
     * Model model2(env)
     * model2.add(x[0]);
     * model2.add(x[1]);
     * model2.add(constraint);
     * ```
     *
     * Note that the default version of the constraint is added to the model (i.e., the constraint is added identical
     * to its first version as defined at creation time).
     *
     * Attention: every variable involved in the default version must already be part of the model.
     * @param t_ctr
     */
    void add(const Ctr& t_ctr);

    /**
     * Adds an existing constraint to the model with different attributes than the default version (i.e., the constraint
     * is added to the model and has the attributes defined in t_temp_ctr).
     *
     * Example:
     * ```cpp
     * Env env;
     *
     * Model model1(env);
     * auto x = model.add_vars(Dim<1>(2), 0, Inf, Continuous, "x");
     * auto constraint = model1.add_ctr(x[0] + 2 * x[1] <= 1.5);
     *
     * Model model2(env)
     * model2.add(x[0]);
     * model2.add(x[1]);
     * model2.add(constraint, x[0] - x[1] >= 0);
     * ```
     *
     * Attention: every variable involved in t_temp_var must already be part of the model.
     *
     * @param t_ctr the constraint to add
     * @param t_temp_ctr the attributes for the constraint in the model
     */
    void add(const Ctr &t_ctr, TempCtr t_temp_ctr);

    /**
     * Returns true if and only if the constraint is part of the model.
     *
     * Example:
     * ```cpp
     * auto c = model.add_ctr(x[0] + 1.5 * x[1] <= 2);
     *
     * std::cout << model.has(c) << std::endl; // output: 1
     * ```
     * @param t_ctr the constraint
     * @return true if and only if the constraint is part of the model
     */
    [[nodiscard]] bool has(const Ctr& t_ctr) const;

    /**
     * Removes a constraint from the model.
     *
     * Example:
     * ```cpp
     * auto c = model.add_ctr(x[0] + 1.5 * x[1] <= 2);
     *
     * std::cout << model.has(c) << std::endl; // output: 1
     *
     * model.remove(c);
     *
     * std::cout << model.has(c) << std::endl; // output: 0
     * ```
     * @param t_ctr
     */
    void remove(const Ctr& t_ctr);

    /**
     * Returns an object able to construct iterators over the set of constraints of the model.
     *
     * Example:
     * ```cpp
     * for (const Ctr& ctr : model.ctrs()) {
     *      std::cout << ctr.name() << std::endl;
     * }
     * ```
     * @return an object able to construct iterators over the set of constraints of the model.
     */
    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const { return m_constraints; }

    /**
     * Returns the unique id (within a given environment) of the model.
     *
     * Example:
     *
     * ```cpp
     * std::cout << model.id() << std::endl;
     * ```
     * @return
     */
    [[nodiscard]] unsigned int id() const { return m_id; }

    /**
     * Creates a new identical model.
     *
     * Example:
     * ```cpp
     * Model* copy = model.clone();
     * // ...
     * delete copy;
     * ```
     * @return a pointer to the new copy
     */
    [[nodiscard]] Model* clone() const;

    [[nodiscard]] Model copy() const;

    void reserve_vars(unsigned int t_size);

    void reserve_ctrs(unsigned int t_size);

    /**
     * Returns the optimization environment of the model
     *
     * Example:
     *
     * ```cpp
     * Env& env = model.env();
     * Var x(env, 0, 1, Binary, "x");
     * ```
     * @return the optimization environment
     */
    [[nodiscard]] Env& env() const { return const_cast<Model*>(this)->m_env; }

    /**
     * Sets the optimizer for the model.
     *
     * The optimizer is directly created using t_optimizer_factory.
     * If an optimizer was already created, it is replaced by the new one.
     *
     * Example:
     * ```cpp
     * model.use(Gurobi());
     * ```
     * @param t_optimizer_factory
     */
    void use(const OptimizerFactory& t_optimizer_factory);

    /**
     * Returns true if and only if the model currently has an optimizer.
     *
     * Example:
     * ```cpp
     * model.use(Gurobi());
     *
     * std::cout << model.has_optimizer() << std::endl; // output: 1
     * ```
     *
     * @return true if and only if the model currently has an optimizer.
     */
    [[nodiscard]] bool has_optimizer() const;

    [[nodiscard]] bool has_optimizer_factory() const { return bool(m_optimizer_factory); }

    [[nodiscard]] const OptimizerFactory& optimizer_factory() const { return *m_optimizer_factory; }

    /**
     * Removes an existing optimizer.
     *
     * The model is therefore left without any optimizer.
     *
     * Example:
     * ```cpp
     * model.use(Gurobi());
     *
     * std::cout << model.has_optimizer() << std::endl; // output: 1
     *
     * model.unuse();
     *
     * std::cout << model.has_optimizer() << std::endl; // output: 0
     * ```
     */
    void unuse();

    /**
     * Adds a vector of optimization objects to the model (i.e., variables or constraints).
     *
     * Example:
     * ```cpp
     * std::vector<std::vector<Var>> x = Var::make_vector(Dim<2>(n, m), 0, 1, Binary, "x");
     *
     * model.add_vector<Var, 2>(x);
     * ```
     * @tparam T the type of optimization objects to add
     * @tparam N the number of dimensions
     * @param t_vector the vector of objects
     */
    template<class T, unsigned int N> void add_vector(const Vector<T, N>& t_vector);

    /**
     * Returns a reference to the model's optimizer.
     *
     * Example:
     * ```cpp
     * std::cout << model.optimizer().as<Optimizers::BranchAndBound>().n_solved_nodes() << std::endl;
     * ```
     * @return a reference to the model's optimizer
     */
    Optimizer& optimizer() { throw_if_no_optimizer(); return *m_optimizer; }

    /**
     * Returns a const reference to the model's optimizer.
     *
     * Example:
     * ```cpp
     * std::cout << model.optimizer().as<Optimizers::BranchAndBound>().n_solved_nodes() << std::endl;
     * ```
     * @return a const reference to the model's optimizer
     */
    [[nodiscard]] const Optimizer& optimizer() const { throw_if_no_optimizer(); return *m_optimizer; }

    /**
     * Solves the model using the model's optimizer
     *
     * Example:
     * ```cpp
     * model.use(Gurobi());
     *
     * model.optimize();
     * ```
     * Attention: an optimizer must have been set beforehand.
     */
    void optimize();

    /**
     * Asks the model's optimizer to write the model (or any useful piece of information) to a file.
     *
     * Example:
     * ```cpp
     * model.use(Gurobi());
     *
     * model.write("model.lp");
     * ```
     *
     * Attention: an optimizer must have been set beforehand.
     * @param t_name the destination file
     */
    void write(const std::string& t_name);

    /**
     * Asks the model's optimizer to update his current version of the model.
     *
     * Note that you typically do not need to call this method since updates are automatically done.
     * While some external-mip perform "lazy updates" and actually update their representation of the model only when necessary
     * (e.g., before solving or writing to file), even in this case, updates are automatically done so that the user
     * should not need to use this method.
     *
     * Example:
     * ```cpp
     * model.add(x);
     *
     * model.update();
     * ```
     *
     * Attention: an optimizer must have been set beforehand.
     */
    void update();

    /**
     * Returns the objective sense of the model (Minimize or Maximize).
     *
     * Example:
     * ```cpp
     * ObjectiveSense sense = model.get_obj_sense();
     *
     * if (sense == Minimize) {
     *      std::cout << "This is a minimization problem" << std::endl;
     * }
     * ```
     * @return the objective sense of the model
     */
    [[nodiscard]] ObjectiveSense get_obj_sense() const;

    /**
     * Returns the objective function of the model.
     *
     * Example:
     * ```cpp
     * const Expr& objective = model.get_obj_expr();
     *
     * std::cout << "The objective function is " << objective << std::endl;
     * ```
     * @return the objective function of the model
     */
    [[nodiscard]] const Expr<Var, Var>& get_obj_expr() const;

    /**
     * Returns the right hand-side of the model.
     *
     * Example:
     * ```cpp
     * const LinExpr<Ctr> rhs = model.get_rhs_expr();
     *
     * std::cout << "The model RHS is " << rhs << std::endl;
     * ```
     *
     * @return the right hand-side of the model
     */
    [[nodiscard]] const LinExpr<Ctr>& get_rhs_expr() const;

    /**
     * Returns a specific coefficient in the model's matrix.
     *
     * Example:
     * ```cpp
     * const Constant& coeff = model.get_mat_coeff(c1, x[0]);
     * std::cout << "The coefficient of x_0 in constraint c1 is " << coeff << std::endl;
     * ```
     *
     * @param t_ctr the constraint corresponding to the row
     * @param t_var the column corresponding to the column
     * @return the matrix coefficient
     */
    [[nodiscard]] const Constant& get_mat_coeff(const Ctr& t_ctr, const Var& t_var) const;

    /**
     * Returns the current status of the optimizer.
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Optimal) {
     *      std::cout << "Optimal solution found!" << std::endl;
     * }
     * ```
     *
     * Attention: an optimizer must have been set beforehand.
     * @return
     */
    [[nodiscard]] SolutionStatus get_status() const;

    /**
     * Returns the reason for the current status of the optimizer.
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (get_status() == Fail) {
     *      std::cout << "The optimizer failed because of " << get_reason() << std::endl; // e.g. output: Numeric
     * }
     * ```
     *
     * Attention: an optimizer must have been set beforehand.
     * @return
     */
    [[nodiscard]] SolutionReason get_reason() const;

    /**
     * Returns the best objective value currently known by the optimizer (i.e., feasible solution objective value)
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Optimal) {
     *     std::cout << "Optimum = " << model.get_best_obj() << std::endl;
     * }
     * ```
     *
     * Attention: an optimizer must have been set beforehand.
     * @return the best objective value currently known by the optimizer
     */
    [[nodiscard]] double get_best_obj() const;

    /**
     * Returns the best bound currently known by the optimizer (e.g., dual bound).
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (get_status() == Feasible) {
     *      const double best_obj = model.get_best_obj();
     *      const double best_bound = model.get_best_bound();
     *      std::cout << "Gap: " << relative_gap(best_bound, best_obj) * 100 << " % << std::endl;
     * }
     * ```
     * @return the best bound currently known by the optimizer
     */
    [[nodiscard]] double get_best_bound() const;

    /**
     * Sets the objective sense of the model.
     *
     * Example:
     * ```cpp
     * model.set_obj_sense(Maximize);
     * ```
     * @param t_value the new objective sense
     */
    void set_obj_sense(ObjectiveSense t_value);

    /**
     * Sets the objective expression of the model.
     *
     * The expression is copied.
     *
     * Example:
     * ```cpp
     * Expr objective = - x[0] - 2 * x[2];
     * model.set_obj_expr(objective);
     * ```
     *
     * Attention: every variable involved in t_objective must already be part of the model.
     * @param t_objective
     */
    void set_obj_expr(const Expr<Var, Var>& t_objective);

    /**
     * Sets the objective expression of the model.
     *
     * The expression is moved.
     *
     * Example:
     * ```cpp
     * model.set_obj_expr(- x[0] - 2 * x[2]);
     * ```
     *
     * Attention: every variable involved in t_objective must already be part of the model.
     * @param t_objective
     */
    void set_obj_expr(Expr<Var, Var>&& t_objective);

    /**
     * Sets the right hand-side of the model
     *
     * The expression is copied.
     *
     * Example:
     * ```cpp
     * Lin<Ctr> rhs = 2 * c1 + 3 * c2;
     *
     * model.set_rhs_expr(rhs); // RHS is then 2 for constraint c1, 2 for constraint c2, 0 for others
     * ```
     *
     * Attention: every constraint involved in t_rhs must already be part of the model.
     * @param t_rhs the right hand-side
     */
    void set_rhs_expr(const LinExpr<Ctr>& t_rhs);

    /**
     * Sets the right hand-side of the model
     *
     * The expression is moved.
     *
     * Example:
     * ```cpp
     * Lin<Ctr> rhs = 2 * c1 + 3 * c2;
     *
     * model.set_rhs_expr(std::move(rhs)); // RHS is then 2 for constraint c1, 2 for constraint c2, 0 for others
     * ```
     *
     * Attention: every constraint involved in t_rhs must already be part of the model.
     * @param t_rhs the right hand-side
     */
    void set_rhs_expr(LinExpr<Ctr>&& t_rhs);

    /**
     * Sets the objective constant.
     *
     * The constant is copied.
     *
     * Example:
     * ```cpp
     * Constant constant = 10;
     * model.set_obj_const(constant);
     * ```
     * @param t_constant the constant
     */
    void set_obj_const(const Constant& t_constant);

    /**
     * Sets the objective constant.
     *
     * The constant is moved.
     *
     * Example:
     * ```cpp
     * model.set_obj_const(10);
     * ```
     * @param t_constant the constant
     */
    void set_obj_const(Constant&& t_constant);

    /**
     * Sets a specific matrix coefficient.
     *
     * The coefficient is copied.
     *
     * Example:
     * ```cpp
     * Constant coeff = 3;
     * model.set_mat_coeff(c1, x[0], coeff); // The coefficient of x_0 in constraint c1 is 3
     * ```
     * @param t_ctr the constraint corresponding to the row
     * @param t_var the variable corresponding to the column
     * @param t_coeff the coefficient
     */
    void set_mat_coeff(const Ctr& t_ctr, const Var& t_var, const Constant& t_coeff);

    /**
     * Sets a specific matrix coefficient.
     *
     * The coefficient is moved.
     *
     * Example:
     * ```cpp
     * model.set_mat_coeff(c1, x[0], 3); // The coefficient of x_0 in constraint c1 is 3
     * ```
     * @param t_ctr the constraint corresponding to the row
     * @param t_var the variable corresponding to the column
     * @param t_coeff the coefficient
     */
    void set_mat_coeff(const Ctr& t_ctr, const Var& t_var, Constant&& t_coeff);

    [[nodiscard]] Ctr get_ctr_by_index(unsigned int t_index) const;

    [[nodiscard]] Var get_var_by_index(unsigned int t_index) const;

    /**
     * Returns the current index of the constraint in the model.
     *
     * Example:
     * ```cpp
     * unsigned int index = model.get_ctr_index(c1);
     *
     * std::cout << "The current index of c1 is " << index << std::endl;
     * ```
     *
     * Note that the index of the constraint may change over time (in particular, when objects are removed from the model)
     * @param t_ctr the constraint
     * @return the constraint's index
     */
    [[nodiscard]] unsigned int get_ctr_index(const Ctr& t_ctr) const;

    /**
     * Returns the type of the constraint in the model
     *
     * Example:
     * ```cpp
     * CtrType.rst type = model.get_ctr_type(c1);
     *
     * if (type == LessOrEqual) {
     *      std::cout << "c1 is a <= constraint" << std::endl;
     * }
     * ```
     * @param t_ctr the constraint
     * @return the type of the constraint
     */
    [[nodiscard]] CtrType get_ctr_type(const Ctr& t_ctr) const;

    /**
     * Returns the row of the constraint in the model.
     *
     * Example:
     * ```cpp
     * const Row& row = model.get_ctr_row(c1);
     *
     * std::cout << "The linear part of c1 is " << row.linear() << std::endl;
     * std::cout << "The quadratic part of c1 is " << row.quadratic() << std::endl;
     * std::cout << "The RHS of c1 is " << row.rhs() << std::endl;
     * ```
     * @param t_ctr the constraint
     * @return the constraint's row
     */
    [[nodiscard]] const Row& get_ctr_row(const Ctr& t_ctr) const;

    /**
     * Returns the dual value of a constraint
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * const double dual = model.get_ctr_dual(c1);
     * ```
     * @param t_ctr the constraint
     * @return the constraint's dual value
     */
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const;

    /**
     * Returns the Farkas certificate of the constraint
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Infeasible) {
     *      std::cout << "The Farkas certificate for c1 is " << model.get_ctr_farkas(c1) << std::endl;
     * }
     * ```
     * @param t_ctr the constraint
     * @return the farkas certificate for the constraint
     */
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const;

    /**
     * Sets a constraint's right hand-side
     *
     * The constant is copied.
     *
     * Example:
     * ```cpp
     * Constant rhs = 10;
     * model.set_ctr_rhs(rhs);
     * ```
     * @param t_ctr the constraint
     * @param t_rhs the right hand-side
     */
    void set_ctr_rhs(const Ctr& t_ctr, const Constant& t_rhs);

    /**
     * Sets a constraint's right hand-side
     *
     * The constant is moved.
     *
     * Example:
     * ```cpp
     * model.set_ctr_rhs(10);
     * ```
     * @param t_ctr the constraint
     * @param t_rhs the constraint's right hand-side
     */
    void set_ctr_rhs(const Ctr& t_ctr, Constant&& t_rhs);

    /**
     * Sets a constraint's type.
     *
     * Example:
     * ```cpp
     * model.set_ctr_type(LessOrEqual);
     * ```
     * @param t_ctr the constraint
     * @param t_type the constraint's type
     */
    void set_ctr_type(const Ctr& t_ctr, CtrType t_type);

    /**
     * Sets a constraint's row.
     *
     * The row is copied.
     *
     * Example:
     * ```cpp
     * Row row(x[0] + 2 * x[1], 1.5);
     * model.set_ctr_row(row);
     * ```
     *
     * Attention: every variable involved in t_row must already be part of the model.
     * @param t_ctr the constraint
     * @param t_row the constraint's row
     */
    void set_ctr_row(const Ctr& t_ctr, const Row& t_row);

    /**
     * Sets a constraint's row.
     *
     * The row is moved.
     *
     * Example:
     * ```cpp
     * Row row(x[0] + 2 * x[1], 1.5);
     * model.set_ctr_row(std::move(row));
     * ```
     *
     * Attention: every variable involved in t_row must already be part of the model.
     * @param t_ctr the constraint
     * @param t_row the constraint's row
     */
    void set_ctr_row(const Ctr& t_ctr, Row&& t_row);

    /**
     * Returns the current index of the variable in the model.
     *
     * Example:
     * ```cpp
     * unsigned int index = model.get_ctr_index(x[0]);
     *
     * std::cout << "The current index of x_0 is " << index << std::endl;
     * ```
     *
     * Note that the index of the variable may change over time (in particular, when objects are removed from the model)
     * @param t_var the variable
     * @return the variable's index
     */
    [[nodiscard]] unsigned int get_var_index(const Var& t_var) const;

    /**
     * Returns the variable's type in the model
     *
     * Example:
     * ```cpp
     * VarType type = model.get_var_type(x[0]);
     *
     * if (type == Continuous) {
     *      std::cout << "x_0 is a continuous variable" << std::endl;
     * }
     * ```
     * @param t_var the variable
     * @return the variable's type
     */
    [[nodiscard]] VarType get_var_type(const Var& t_var) const;

    /**
     * Returns the lower bound of a variable in the model
     *
     * Example:
     * ```cpp
     * const double lb = model.get_var_lb(x[0]);
     *
     * std::cout << "x_0 >= " << lb << " is part of the model" << std::endl;
     * ```
     * @param t_var the variable
     * @return the variable's lower bound
     */
    [[nodiscard]] double get_var_lb(const Var& t_var) const;

    /**
     * Returns the upper bound of a variable in the model
     *
     * Example:
     * ```cpp
     * const double ub = model.get_var_ub(x[0]);
     *
     * std::cout << "x_0 <= " << ub << " is part of the model" << std::endl;
     * ```
     * @param t_var the variable
     * @return the variable's upper bound
     */
    [[nodiscard]] double get_var_ub(const Var& t_var) const;

    /**
     * Returns the primal value of a variable in the model
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Optimal) {
     *      std::cout << "Primal value of x_0 is " << model.get_var_primal(x[0]) << std::endl;
     * }
     * ```
     * @param t_var the variable
     * @return the primal value
     */
    [[nodiscard]] double get_var_primal(const Var& t_var) const;

    /**
     * Returns the reduced cost of a variable in the model
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Optimal) {
     *      std::cout << "Reduced cost of x_0 is " << model.get_var_reduced_cost(x[0]) << std::endl;
     * }
     * ```
     * @param t_var the variable
     * @return the reduced cost
     */
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const;

    /**
     * When the model is unbounded, returns the ray value of a variable of the model
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * if (model.get_status() == Unbounded) {
     *      std::cout << "Ray value of x_0 is " << model.get_var_ray(x[0]) << std::endl;
     * }
     * ```
     * @param t_var the variable
     * @return the ray value
     */
    [[nodiscard]] double get_var_ray(const Var& t_var) const;

    /**
     * Returns the column of a variable in the model.
     *
     * Example:
     * ```cpp
     * const Column& column = model.get_var_column(t_var);
     *
     * std::cout << "The objective coefficient of x_0 is " << column.obj() << std::endl;
     * ```
     * @param t_var the variable
     * @return the variable's column
     */
    [[nodiscard]] const Column& get_var_column(const Var& t_var) const;

    /**
     * Sets a variable's type in the model.
     *
     * Example:
     * ```cpp
     * model.set_var_type(x[0], Binary); // x_0 is now binary
     * ```
     * @param t_var the variable
     * @param t_type the variable's type
     */
    void set_var_type(const Var& t_var, VarType t_type);

    /**
     * Sets a variable's lower bound in the model
     *
     * Example:
     * ```cpp
     * model.set_var_lb(x[0], 1); // Set the restriction x_0 >= 1
     * ```
     * @param t_var the variable
     * @param t_lb the variable's lower bound
     */
    void set_var_lb(const Var& t_var, double t_lb);

    /**
     * Sets a variable's upper bound in the model
     *
     * Example:
     * ```cpp
     * model.set_var_ub(x[0], 0); // Set the restriction x_0 <= 1
     * ```
     * @param t_var the variable
     * @param t_lb the variable's upper bound
     */
    void set_var_ub(const Var& t_var, double t_ub);

    /**
     * Sets a variable's objective coefficient in the model.
     *
     * The constant is copied.
     *
     * Example:
     * ```cpp
     * Constant obj = 10;
     * model.set_var_obj(x[0], obj);
     * ```
     * @param t_var the variable
     * @param t_obj the variable's objective coefficient
     */
    void set_var_obj(const Var& t_var, const Constant& t_obj);

    /**
     * Sets a variable's objective coefficient in the model.
     *
     * The constant is moved.
     *
     * Example:
     * ```cpp
     * model.set_var_obj(x[0], 10);
     * ```
     * @param t_var the variable
     * @param t_obj the variable's objective coefficient
     */
    void set_var_obj(const Var& t_var, Constant&& t_obj);

    /**
     * Sets a variable's column in the model.
     *
     * The column is copied.
     *
     * Example:
     * ```cpp
     * Column column(10);
     * model.set_var_column(x[0], column);
     * ```
     *
     * Attention: every constraint involved in t_column must already be part of the model.
     * @param t_var the variable
     * @param t_column the variable's column
     */
    void set_var_column(const Var& t_var, const Column& t_column);

    /**
     * Sets a variable's column in the model.
     *
     * The column is moved.
     *
     * Example:
     * ```cpp
     * Column column(10);
     * model.set_var_column(x[0], std::move(column));
     * ```
     *
     * Attention: every constraint involved in t_column must already be part of the model.
     * @param t_var the variable
     * @param t_column the variable's column
     */
    void set_var_column(const Var& t_var, Column&& t_column);

    /**
     * Returns the number of available primal solutions.
     *
     * Some external-mip can find several optimal or sub-optimal solutions (e.g., in branch-and-bound algorithms with
     * solution pools). This function can be used to query the number of available solutions.
     * The solution is selected with `Model::set_solution_index` and retrieved via get_var_primal and get_best_obj.
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * std::cout << n_solutions << " solution(s) have been found" << std::endl;
     * ```
     *
     * @return the number of available solution primal solutions
     */
    [[nodiscard]] unsigned int get_n_solutions() const;

    /**
     * Returns the current solution index.
     *
     * Some external-mip can find several optimal or sub-optimal solutions (e.g., in branch-and-bound algorithms with
     * solution pools). This function return the current solution index which is being returned by the model.
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * std::cout << model.get_solution_index() << std::endl; // output: 0
     * ```
     * @return
     */
    [[nodiscard]] unsigned int get_solution_index() const;

    /**
     * Sets the index of the solution returned by get_var_primal, get_status and get_reason.
     *
     * Some external-mip can find several optimal or sub-optimal solutions (e.g., in branch-and-bound algorithms with
     * solution pools). This method can be used to change the solution index which is being returned by the model.
     *
     * Example:
     * ```cpp
     * model.optimize();
     *
     * const unsigned int n_solutions = model.get_n_solutions();
     *
     * std::cout << n_solutions << " solution(s) have been found" << std::endl;
     *
     * for (unsigned int i = 0 ; i < n_solutions ; ++i) {
     *      model.set_solution_index(i);
     *      std::cout << save_primal(model) << std::endl;
     * }
     * ```
     *
     * @return the number of available solution primal solutions
     */
    void set_solution_index(unsigned int t_index);

    void scale_to_integers(unsigned int t_n_digits);

    Model fix(const Solution::Primal& t_primals) const;
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
idol::Vector<idol::Var, N> idol::Model::add_vars(Dim<N> t_dim, double t_lb, double t_ub, VarType t_type, const std::string& t_name) {
    auto result = Var::make_vector(m_env, t_dim, t_lb, t_ub, t_type, t_name);
    add_vector<Var, N>(result);
    return result;
}

template<unsigned int N>
idol::Vector<idol::Ctr, N> idol::Model::add_ctrs(Dim<N> t_dim, CtrType t_type, const Constant &t_constant, const std::string &t_name) {
    auto result = Ctr::make_vector(m_env, t_dim, t_type, t_constant, t_name);
    add_vector<Ctr, N>(result);
    return result;
}

namespace idol {

    static auto save_primal(const Model &t_original_model, const Model &t_model) {

        Solution::Primal result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Optimal && status != Feasible && status != SubOptimal) {
            throw Exception("Primal values not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(t_model.get_best_obj());

        for (const auto &var: t_original_model.vars()) {
            result.set(var, t_model.get_var_primal(var));
        }

        return result;

    }

    static auto save_primal(const Model &t_original_model) {
        return save_primal(t_original_model, t_original_model);
    }

    static auto save_ray(const Model &t_original_model, const Model &t_model) {

        Solution::Primal result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Unbounded) {
            throw Exception("Ray not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(-Inf);

        for (const auto &var: t_original_model.vars()) {
            result.set(var, t_model.get_var_ray(var));
        }

        return result;

    }

    static auto save_ray(const Model &t_original_model) {
        return save_ray(t_original_model, t_original_model);
    }

    static auto save_dual(const Model &t_original_model, const Model &t_model) {

        Solution::Dual result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Optimal && status != Feasible) {
            throw Exception("Dual values not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(t_model.get_best_bound());

        for (const auto &ctr: t_original_model.ctrs()) {
            result.set(ctr, t_model.get_ctr_dual(ctr));
        }

        return result;

    }

    static auto save_dual(const Model &t_original_model) {
        return save_dual(t_original_model, t_original_model);
    }

    static auto save_farkas(const Model &t_original_model, const Model &t_model) {

        Solution::Dual result;

        const auto status = t_model.get_status();
        const auto reason = t_model.get_reason();

        if (status != Infeasible) {
            throw Exception("Farkas certificate not available.");
        }

        result.set_status(status);
        result.set_reason(reason);

        result.set_objective_value(+Inf);

        for (const auto &ctr: t_original_model.ctrs()) {
            result.set(ctr, t_model.get_ctr_farkas(ctr));
        }

        return result;

    }

    static auto save_farkas(const Model &t_original_model) {
        return save_farkas(t_original_model, t_original_model);
    }

}

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const idol::Model &t_model) {

        using namespace idol;

        LimitedWidthStream stream(t_os, 560);

        if (t_model.get_obj_sense() == Minimize) {
            stream << "Minimize";
        } else {
            stream << "Maximize";
        }

        stream << "\n\t" << t_model.get_obj_expr() << "\nSubject To\n";

        for (const auto &ctr: t_model.ctrs()) {

            const auto &row = t_model.get_ctr_row(ctr);
            const auto &linear = row.linear();
            const auto &quadratic = row.quadratic();
            const auto type = t_model.get_ctr_type(ctr);

            stream << '\t' << ctr << ": ";

            if (linear.empty()) {
                stream << quadratic;
            } else {
                stream << linear;
                if (!quadratic.empty()) {
                    stream << " + " << quadratic;
                }
            }

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

            stream << row.rhs() << '\n';
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

            stream << '\n';
        }

        if (!generals.empty()) {
            stream << "Generals\n";
            for (const auto& var : generals) {
                stream << '\t' << var.name() << '\n';
            }
        }

        if (!binaries.empty()) {
            stream << "Binaries\n";
            for (const auto& var : binaries) {
                stream << '\t' << var.name() << '\n';
            }
        }

        return t_os;
    }

}

#endif //IDOL_MODEL_H
