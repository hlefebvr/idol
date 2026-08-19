\page lib_milp_basics Basics
\brief Describes the basic usage of `idol` and shows how to model a MILP and solve it.

\tableofcontents

\section lib_milp_basics_hello_world A Toy Example

To illustrate the modeling capabilities of `idol`, we begin with a small example given by the MILP

\f[
\begin{aligned}
    \min_{x,y} \quad & -x - 2y \\
    \text{s.t.} \quad & -x + y \le 1, \\
    & 2x + 3y \le 12, \\
    & 3x + 2y \le 12, \\
    & x,y\in\mathbb{Z}_{\ge 0}.
\end{aligned}
\f]

This is a minimization problem which involves two integer variables, \\( x \\) and
\\( y \\), both constrained to be non-negative. The feasible region is defined by
three linear inequalities.

The continuous relaxation of this problem (where \\( x \\) and \\( y \\) are allowed to
take real values) has a unique solution at \\( (x^*, y^*) = (2.4, 2.4) \\), with
objective value \\( -7.2 \\). The original integer-constrained problem also admits a
unique solution, which is attained at \\( (x^*, y^*) = (2, 2)\\), with objective
value \\( -5 \\).

Modeling this problem in `idol` is straightforward and will look familiar if you use other optimization frameworks such as JuMP
in Julia or the Gurobi C++, or even Python, interface.

The following code snippet is largely self-explanatory.

```cpp
#include <iostream>
#include "idol/modeling.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Create a new model.
    Model model(env);

    // Create decision variables x and y.
    const auto x = model.add_var(0, Inf, Integer, -1, "x");
    const auto y = model.add_var(0, Inf, Integer, -2, "y");

    // Create constraints.
    const auto c1 = model.add_ctr(-x + y <= 1);
    const auto c2 = model.add_ctr(2 * x + 3 * y <= 12);
    const auto c3 = model.add_ctr(3 * x + 2 * y <= 12);
    
    return 0;
}

```

Let's walk through this code. First, we create a new
optimization environment that will store all our optimization objects such as
variables or constraints. This is done with the `Env` class.
Destroying an environment automatically destroys all
objects which were created with this environment. 

Then, we create an optimization model using the `Model` class. In `idol`, all
models are for minimization problems. Decision variables are created in
using the `Model::add_var` method. There, we
set the lower bound to 0 and an infinite upper bound using the defined
constant `Inf`. Both variables are defined as `Integer`.
Note that other types are possible, e.g., `Continuous` and
`Binary`. The objective coefficients are also set in these lines by the
fourth argument (note that this can also be done afterward). 
The last argument corresponds to the internal name of that
variable and is mainly used for debugging and printing. 

Finally, we add constraints to the model to define the feasible region of the problem.
To do this, we use the `Model::add_ctr` method.

Note that at this stage we are only formulating
the problem; no optimization is being performed yet.
Solving the model is the focus of the section.

\section lib_milp_basics_solving Using an External Solver

To solve a model, we need to attach an optimizer to it. This is done using the `Model::use` method that takes a single argument.
This argument is what is called an optimizer factory, i.e., an object that will create on demand an actual optimizer
for solving the model. 

Here, we continue ongoing example and illustrate how to use the commercial solver Gurobi
to compute a solution to this problem. To do so, we use the `Gurobi` class, which is an optimizer factory
for the optimizer `Optimizers::Gurobi` that interfaces with the Gurobi C API.

Here is a code snippet.

```cpp
model.use(Gurobi());
model.optimize();

const auto status = model.get_status();

std::cout << "Status: " << status << std::endl;
std::cout << "Reason: " << model.get_reason() << std::endl;

if (status == Optimal || status == Feasible) {
    std::cout << "Best Obj. " << model.get_best_obj() << std::endl;
    std::cout << "x = " << model.get_var_primal(x) << std::endl;
    std::cout << "y = " << model.get_var_primal(y) << std::endl;
}
```

First, we tell `idol` to "use" the Gurobi optimizer factory for this model. 
Then, we call the `Model::optimize` method that, indeed, creates and calls an optimizer to solve the problem.
Finally, we retrieve some basic information regarding the solution of our problem.
If the status is `Optimal` or `Feasible`, we print out the primal values.

> Obviously, Gurobi must be installed on your machine for this code to successfully run.
> Note that Gurobi is dynamically loaded by `idol` provided that your `GUROBI_HOME` environment variable
> is correctly set up.

Note that there are other optimizers, for instance, here is how to use HiGHS instead.

```cpp
model.use(HiGHS());
```

\section lib_milp_basics_advanced More Advanced Topics

The previous sections should be enough for you to get started with the library. 

Next is a list of more in-depth pages about basic concepts in `idol`. 

- \subpage lib_milp_basics_advanced_environment
- \subpage lib_milp_basics_advanced_models
- \subpage lib_milp_basics_advanced_variables
- \subpage lib_milp_basics_advanced_expressions
- \subpage lib_milp_basics_advanced_constraints
- \subpage lib_milp_basics_advanced_objectives
- \subpage lib_milp_basics_advanced_optimizers
- \subpage lib_milp_basics_advanced_create_optimizer

\page lib_milp_basics_advanced_environment The Optimization Environment
\brief Describes what is an optimization environment and how it is used.
\tableofcontents

\section lib_milp_basics_advanced_environment_what What is an Optimization Environment

Any optimization object, such as variables, constraints and models, are
managed through a central entity called an "optimization environment". This
environment is represented by the `Env` class. It acts as a container
and controller for all optimization-related objects created within its scope.

The environment has two primary responsibilities:
- **Lifecycle Management.** When an environment is destroyed, all
  objects created by this environment are automatically deleted. This
  eliminates the need for manual memory management. Also, once an object is
  no longer referenced, it is safely cleaned up by the environment, i.e.,
  you do not need to manually delete objects.
- **Version Tracking.** During the execution of an optimization
  program, objects like variables and constraints may appear in different
  models with model-specific changes. These different versions of a single
  object are all stored and managed in the environment.

Typically, a single environment should suffice for your application. While
`idol` technically allows the creation of multiple environments, this
is strongly discouraged because this is a source of error. 
Objects created in one environment must not be mixed
with those from another. For example, adding a variable from one environment
to a model created by a different environment will lead to an undefined
behavior, often resulting in a segmentation fault and a program crash.

Creating an environment is straightforward.

```cpp
Env env; // Creates a new optimization environment.
```

Once initialized, the environment can be used to create models, variables, and
constraints. All such objects are associated with `env` and are managed
by it throughout their lifetime.

\section lib_milp_basics_advanced_environment_tolerances Handling of Default Tolerances and Parameters

Another role of the optimization environment is to store default values for parameters and tolerances used by underlying methods.
In practice, however, it is often more relevant to modify these parameters at the optimizer level.

For instance, the following code snippet sets the default tolerance for checking that the optimality gap is closed at the environment level.
Unless specified otherwise, all methods used to solve problems created with this environment will use this value.

```cpp
Env env;
env.set_tol_mip_relative_gap(1e-3);
```

On the other hand, the following code snippet modifies the gap tolerance only for one given optimizer. 

```cpp
auto glpk = GLPK();
glpk.with_tol_mip_relative_gap(1e-3);

model.use(glpk);
```

Here is a list of tolerances that can be adjusted at an environment level.

| Name                    | Description                                          |
|-------------------------|------------------------------------------------------|
| `tol_mip_relative_gap`  | Relative optimality gap tolerance                    |
| `tol_mip_absolute_gap`  | Absolute optimality gap tolerance                    |
| `tol_integer`           | Integrality tolerance                                |
| `tol_feasibility`       | Feasibility tolerance for constraints                |
| `tol_optimality`        | Optimality tolerance (used for reduced cost testing) |

Here is a list of parameters that can be adjusted at an environment level.

| Name                                 | Description                                             |
|--------------------------------------|---------------------------------------------------------|
| `param_logs`                         | Enable or disable solver logs                           |
| `param_presolve`                     | Enable or disable presolve                              |
| `param_time_limit`                   | Time limit (in seconds)                                 |
| `param_thread_limit`                 | Maximum number of threads                               |
| `param_best_bound_stop`              | Stop when a given bound is reached                      |
| `param_best_obj_stop`                | Stop when a given objective value is reached            |
| `param_iteration_limit`              | Maximum number of iterations                            |
| `param_infeasible_or_unbounded_info` | Enable detailed infeasible/unbounded information        |

To modify one of these, simply use the corresponding the `Env::set_*` method. Similarly, reading it is done with the
corresponding `Env::get_*` method. 

Here is an example to impose a thread limit of one on all underlying methods. 

```cpp
env.set_thread_limit(5);
std::cout << "Using at most " << env.get_thread_limit(); << " threads." << std::endl; 
```

\page lib_milp_basics_advanced_models Models
\brief Describes how models are represented in `idol`.
\tableofcontents

\section lib_milp_basics_advanced_models_what What is a Model and What Can You Do with It?

Mathematical optimization problems are modeled using the `Model` class.
A model consists of a collection of variables and constraints together with an
objective function. It is created by invoking the constructor of
`Model` and passing an environment as its first argument.

```cpp
Env env;
Model model(env); // Creates an empty model.
```

Here, we first create a new optimization environment, then create an
optimization model. Note that the newly created model does not yet contain any
variable or constraint. All models are treated as minimization
problems. This is not a real restriction, however, since
\\( \max_{x\in X} f(x) = - \min_{x\in X} -f(x) \\) holds for any function \\( f \\) and
any set \\( X \\).

Another way to create a model is by importing it from an `.mps` or
an `.lp` file. To do this, you will need to rely on an external solver.
In what follows, we use `GLPK`, which is an open-source solver which
can be easily installed on your computer.

```cpp
Env env;
auto model = GLPK::read_from_file("/path/to/some/file.mps");
```

The decision to rely on external solvers is justified by two main
considerations. First, `idol` is typically used in combination with
such solvers anyway to efficiently solve optimization problems. Second, it is
generally safer to leverage well-established solver implementations, which
have been extensively tested over many years, to handle any mistake
or ambiguity.

Now that we have a model imported, we can safely iterate over its variables
and constraints. This can be done as follows.

```cpp
for (const auto& var : model.vars()) {
    std::cout << var.name() << std::endl;
}
```

Here, we use the `Model::vars` method to get access to the variables
of the model and write down their names. Note that you can also use the
`operator<<(std::ostream\&, const Model\&)` function to print the model
to the console, or to a file. This can be useful for debugging.

Once we have iterated over the variables, we may want to iterate over
constraints as well. To do so, we can use the `Model::ctrs` method
for linear constraints, the `Model::qctrs` method for quadratic
constraints, and the `Model::sosctrs()` method for SOS-type
constraints. The next code snippet shows how to get the number of variables
and constraints in the model.

```cpp
std::cout << "N. of vars: "        << model.vars().size()    << '\n';
std::cout << "N. of linear ctrs: " << model.ctrs().size()    << '\n';
std::cout << "N. of quad. ctrs: "  << model.qctrs().size()   << '\n';
std::cout << "N. of SOS ctrs: "    << model.sosctrs().size() << '\n';
```

To obtain model-specific information about a variable, a constraint, or the
objective function, `idol` provides methods of the form
`Model::get_<X>_<Y>(const <T>&)`, where `X` denotes the type of
object—such as `var`, `ctr`, `qctr`, `sosctr`, or
`obj`, and `Y` specifies the particular attribute to access, for
example `lb`, `type`, or `column` for variables. To give a specific
example, the following code snippet counts the number of binary variables in
the model.

```cpp
unsigned int n_binary_vars = 0;

// Iterate over all variables in the model.
for (const auto& var : model.vars()) {

    // Get the variable type in this model.
    const auto type = model.get_var_type(var);

    // Check type is binary.
    if (type == Binary) {
        ++n_binary_vars;
    }

}
```

The complete set of information that can be accessed through a model is
described in detail in the respective page, i.e., that on variables, constraints or objective function.

\section lib_milp_basics_advanced_models_copy Copying and Moving Models

In most practical situations, it is preferable to avoid copying a model. Instead, one should
often pass a reference to it to auxiliary functions. For this reason, the
copy constructor of the `Model` class is declared `private`. If
copying a model is indeed required, the `Model::copy` method and the
move constructor can be used. The following example illustrates this usage.

```cpp
Model copy_and_relax_integrality(const Model& t_model) {
    
    auto result = t_model.copy();
    
    for (const auto& var : result.vars()) {
        result.set_var_type(var, Continuous);
    }
    
    return std::move(result);
}

const auto model = Gurobi::read_from_file("problem.lp");
auto continuous_relaxation = copy_and_relax_integrality(model);
```

Here, `continuous_relaxation` is now an independent copy of the original model and is
being modified without altering its source model.

\page lib_milp_basics_advanced_variables Variables
\brief Describes how optimization variables are handled in `idol` and how to use them.
\tableofcontents

Variables constitute the decision-making elements of an optimization problem.
They represent the quantities to be determined in order to optimize an
objective function, subject to a set of constraints. In `idol`,
variables are represented by the `Var` class.

\section lib_milp_basics_advanced_variables_create Creating Variables

Creating variables can mainly be done in two ways. The first approach uses the
`Var` constructor and added to a model with the `Model::add` method.
The second one relies on the `Model::add_var` methods. 

\subsection lib_milp_basics_advanced_variables_create_env Using the Variable Constructor

We begin
with the first approach, which employs the `Var` constructor. Although
this method is somewhat less direct, it gives greater insight into how
optimization objects are managed within `idol`.

We focus on the constructor given by
```cpp
Var(Env&, double, double, VarType, double, std::string)
```

The constructor requires six arguments. The first specifies the optimization
environment, which manages the variable's lifetime and versions. The next two
arguments define the lower and upper bounds of the variable, which can be set
to infinity using `Inf`. The fourth argument indicates the type
of the variable, for example `Continuous`, `Integer`,
or `Binary`. The fifth argument provides the linear coefficient of
the variable in the objective function, and the sixth argument assigns a name
to the variable.

For example, the following code creates a new variable within the environment.

```cpp
Var x(env, 0, Inf, Continuous, 2, "x");
```

This variable is continuous, non-negative, and has an objective coefficient of
2. It is named "x". Importantly, at this stage the variable does
not belong to any model. What has been created is referred to as the "default
version" of the variable. By default, if this variable is later added to a
model, it will have these attributes within that model. For example, the
following code demonstrates how to create the variable and add it to a model.

```cpp
// Create a variable in the environment.
Var x(env, 0, Inf, Continuous, 2, "x");

// Add the variable to a model
model.add(x);
```

By default, the variable "x" is added to the model as a continuous,
non-negative variable with an objective coefficient of 2. Other constructors
are also available in the `Var` class. For example, it is possible to
provide a column associated with the variable so that it is automatically
incorporated into the constraint matrix. Columns can be constructed using the
`LinExpr<Ctr>` class in a straightforward and intuitive manner. For
more details, see the page dedicated to expressions, in the same tutorial.
We provide one illustrative example here.

```cpp
// This function is assumed to return a vector of constraints.
const std::vector<Ctr> ctrs = get_vector_of_ctrs();

// Create the column associated to x.
LinExpr<Ctr> column = -1 * c[0] + 2 * c[1] + 3 * c[2];

// Create a variable in the environment.
Var x(env, 0, Inf, Integer, -1, std::move(column), "x");

// Add the variable to a model.
model.add(x);
```

Finally, note that it is possible to avoid adding the default version to a
model by overriding it as follows.

```cpp
// Add the variable to a model, overriding the default version.
model.add(x, TempVar(0, Inf, Continuous, 2, LinExpr<Var>()));
```

Here, we note the use of the `TempVar` class. This lightweight class is
designed to represent a variable that has not yet been created within an
environment. It stores all the attributes of the variable to be created, but
cannot be used for any other purpose except for holding these attributes and
instantiating an actual variable.

\subsection lib_milp_basics_advanced_variables_create_model Using the Model

The second approach for creating variables is more straightforward, although
it is internally equivalent to the method described above. This approach uses
the `Model::add_var` methods of the `Model` class. The
following code snippet illustrates this usage and should be easy to
understand.

```cpp
const auto x = model.add_var(0, Inf, Continuous, 2, "x");
```

Note that it is not necessary to pass the environment explicitly, because the
environment associated with the model is used automatically. In this single
call, two operations are performed: first, a default version of the variable
is created, and second, the variable is added to the model. Similarly, it is
also possible to add a variable together with a specific column in the constraint
matrix.

\subsection lib_milp_basics_advanced_variables_create_array Creating Multiple Variables at Once

In some cases, it is more convenient to create multiple variables at once.
This can be accomplished using the `Var::make_vector` function or the
`Model::add_vars` method. Both functions require an additional
parameter specifying the dimension of the variable set. For example, the
following illustrates how to create a set of variables indexed by a \\( 2\times 3 \\) grid.

```cpp
// Create a (2,3) "vector" of variables.
const auto x = Var::make_vector(env, Dim<2>(2, 3), 0, Inf, Continuous, "x");

// Add all variables
model.add_vector<Var, 2>(x);

// Print the first variable's name.
std::cout << "x_0_0 = " << x[0][0].name() << std::endl;
```

Note that we use the `Dim` class to specify the dimensions of the
variable set. The `Dim` class is a template that takes an integer
parameter, which indicates the number of indices for the new variable. In this
example, we pass 2 to create a two-dimensional index. The size of each
dimension is then specified by providing the appropriate arguments to the
constructor of `Dim`, namely 2 and 3.

Alternatively, the same result can be achieved using methods of the
`Model` class. The following snippet illustrates this approach.

```cpp
const auto x = model.add_vars(Dim<2>(2,3), 0, Inf, Continuous, "x");
```

\section lib_milp_basics_advanced_variables_remove Removing Variables

Once a variable has been added to a model, it can also be removed using the
`Model::remove` method. This operation removes the
variable from the model and updates all linear and quadratic constraints in
which the variable appears. Attempting to remove a variable that does not
belong to the model will result in an exception. To check whether a variable
is part of a model, the `Model::has` method can be used;
it returns `true` if and only if the variable is part of the model.

Note that variables involved in SOS-type constraints cannot be removed
directly. This restriction is not limiting in practice, as SOS constraints
themselves can be removed and added again if needed.

\section lib_milp_basics_advanced_variables_access Accessing Variables

Variables possess two immutable attributes: a name, assigned at the
time of creation, and a unique id within the environment. Other
attributes are model-specific and can be accessed through the model's methods
`Model::get_var_<Y>`, where `<Y>` denotes the name of the
attribute. The following list summarizes the methods available for retrieving
information about variables in a model.

| Method | Description                                                             |
|--------|-------------------------------------------------------------------------|
| lb     | Returns the lower bound of the variable. May lie in [-`Inf`, `Inf`].    |
| ub     | Returns the upper bound of the variable. May lie in ([-`Inf`, `Inf`].    |
| obj    | Returns the objective coefficient in the linear part of the objective.  |
| type   | Returns the variable type: `Continuous`, `Integer`, or `Binary`.        |
| column | Returns the associated column in the constraint matrix.                 |
| index  | Returns the index of the variable (may change if variables are removed). |

The following example demonstrates how to print all free variables in a model.

```cpp
for (const auto& var : model.vars()) {

    const double lb = model.get_var_lb(var);
    const double ub = model.get_var_ub(var);

    if (is_neg_inf(lb) && is_pos_inf(ub)) {
        std::cout << var.name() << " is free." << std::endl;
    }

}
```

One final note regarding variable indices. Although indices may change over
time, e.g., if variables are removed from a model, they can still be used to
access variables via the `Model::get_var_by_index` method. The
following code snippet illustrates an alternative way to iterate over all
variables in a model.

```cpp
for (unsigned int i = 0, n = model.vars().size(); i < n; ++i) {

    // Get the variable by index
    const auto& var = model.get_var_by_index(i);
    
    // Print out its name
    std::cout << var.name() << std::endl;

}
```

\section lib_milp_basics_advanced_variables_modify Modifying Variables

Some attributes of a variable can be modified directly through the model's
methods `Model::set_var_<Y>`, where `<Y>` denotes the attribute to
be changed. The list of attributes is exactly those presented in the previous section.

We conclude with an example showing how to change the objective coefficient of a given variable.

```cpp
model.set_var_obj(x[0], 5); // sets the objective coefficient of x_0 to 5
```

\page lib_milp_basics_advanced_expressions Expressions
\brief Describes how mathematical expressions are represented and built in `idol`.
\tableofcontents

Expressions are the building blocks of objective functions, constraints, rows,
and columns. In ordinary modeling code, they are created naturally by combining
variables, constants, and the usual arithmetic operators. The header
`idol/modeling.h` provides the expression types and their operators.

\section lib_milp_basics_advanced_expressions_types Expression Types

`idol` exposes three main expression categories.

| Type | Represents | Main parts |
|------|------------|------------|
| `LinExpr<T>` | A linear combination of objects of type `T` | Terms \\( a_i t_i \\) |
| `AffExpr<T>` | A linear expression plus a constant | `linear()` and `constant()` |
| `QuadExpr<T>` | Quadratic terms plus an affine expression | Quadratic terms and `affine()` |

For model expressions, the template argument is normally `Var`. Thus,
`LinExpr<Var>`, `AffExpr<Var>`, and `QuadExpr<Var>` represent linear, affine,
and quadratic functions of decision variables. `LinExpr` is also used with
other optimization objects. In particular, `LinExpr<Ctr>` represents a column
of the linear constraint matrix.

The distinction between linear and affine expressions matters because a
`LinExpr` has no constant term. For example, \\( 2x-y \\) is linear, while
\\( 2x-y+3 \\) is affine. Multiplying variables produces a `QuadExpr`.

\section lib_milp_basics_advanced_expressions_create Building Expressions

The most convenient way to create expressions is with overloaded arithmetic
operators. Suppose that `x`, `y`, and `z` are variables in a model.

```cpp
LinExpr<Var> linear = 2 * x - 3 * y + z;
AffExpr<Var> affine = 4 + linear;
QuadExpr<Var> quadratic = x * x + 2 * x * y + affine;
```

Addition and subtraction combine compatible expressions. Multiplication by a
scalar and division by a scalar preserve the expression category. A product of
two variables, or of a variable and a linear or affine expression, creates a
quadratic expression.

Expressions are value objects and can be assembled incrementally.

```cpp
LinExpr<Var> activity;
activity += 2 * x;
activity -= y;
activity += z;

AffExpr<Var> shifted = activity;
shifted.constant() = 5;
```

The `linear()` and `constant()` accessors give access to the two parts of an
affine expression. The affine part of a quadratic expression is available
through `QuadExpr::affine()`. The quadratic terms themselves are stored in the
`QuadExpr` base linear expression, indexed by unordered pairs of objects.

\section lib_milp_basics_advanced_expressions_sum Summing Indexed Terms

For indexed models, the `idol_Sum` helper builds an expression by evaluating a
term over an iterable range. The following example creates
\\( \sum_{i=0}^{n-1} c_i x_i \\).

```cpp
const unsigned int n = 4;
const std::vector<double> cost { 1, 3, 2, 5 };
const auto x = model.add_vars(Dim<1>(n), 0, Inf, Continuous, 0, "x");

const auto total_cost = idol_Sum(i, Range(n), cost[i] * x[i]);
model.set_obj_expr(total_cost);
```

`Range(n)` iterates from zero up to, but not including, `n`. The iterable
passed to `idol_Sum` must be non-destructive to traverse, and an empty range
produces a zero expression of the inferred type.

\section lib_milp_basics_advanced_expressions_access Inspecting and Modifying Terms

Linear expressions are sparse containers. They can be iterated over and their
coefficients can be read or changed with `get` and `set`.

```cpp
LinExpr<Var> expression = 2 * x + 3 * y;

std::cout << "coefficient of x: " << expression.get(x) << std::endl;
expression.set(y, -1);

for (const auto& [var, coefficient] : expression) {
    std::cout << coefficient << " * " << var.name() << std::endl;
}
```

Querying a missing term with `get` returns zero. Setting a coefficient to a
numerically zero value removes that sparse term. The same iteration pattern
applies to the quadratic part of a `QuadExpr`; each key is a
`CommutativePair<Var>` with `first` and `second` members.

Expressions are not attached to a model by themselves. When an expression is
used as an objective, row, column, or quadratic constraint, construct it from
the optimization objects that belong to the corresponding model.

\page lib_milp_basics_advanced_constraints Constraints
\brief Describes linear, quadratic, and special ordered set constraints in `idol`.
\tableofcontents

Constraints restrict the feasible solutions of a model. `idol` represents
linear constraints with `Ctr`, quadratic constraints with `QCtr`, and special
ordered set constraints with `SOSCtr`. These objects have an identity in the
optimization environment, while their expressions and other attributes are
stored as model-specific versions.

\section lib_milp_basics_advanced_constraints_linear Linear Constraints

A linear constraint such as \\( a^\top x \le b \\) is represented in C++ by
writing `<=`, `>=`, or `==` between affine expressions. The comparison creates
a lightweight `TempCtr`, which `Model::add_ctr` turns into a `Ctr` associated
with the model.

```cpp
Env env;
Model model(env);

const auto x = model.add_var(0, Inf, Continuous, 0, "x");
const auto y = model.add_var(0, Inf, Continuous, 0, "y");

const auto capacity = model.add_ctr(2 * x + y <= 10, "capacity");
const auto balance = model.add_ctr(x - y == 0, "balance");
const auto demand = model.add_ctr(x + y >= 3, "demand");
```

Constants may appear on either side. `TempCtr` moves all variable terms to its
left-hand side and stores a separate right-hand side. The possible `CtrType`
values are `LessOrEqual`, `GreaterOrEqual`, and `Equal`.

A constraint can instead be created in the environment and added later. As
with variables, this creates a default version that a model uses when the
object is added.

```cpp
Ctr linking(env, x + 2 * y <= 8, "linking");
model.add(linking);
```

The expression must reference variables already known to the model.

\subsection lib_milp_basics_advanced_constraints_linear_access Accessing and Modifying Linear Constraints

The row, right-hand side, type, and model index are accessed through the
model. The matrix coefficient API provides a convenient way to update a
single term.

```cpp
const auto row = model.get_ctr_row(capacity);
const double rhs = model.get_ctr_rhs(capacity);
const auto type = model.get_ctr_type(capacity);

model.set_mat_coeff(capacity, y, 4);
model.set_ctr_rhs(capacity, 12);
model.set_ctr_type(capacity, GreaterOrEqual);
model.set_ctr_row(capacity, 3 * x + 4 * y);
```

`Model::ctrs()` iterates over the linear constraints. `Model::has` tests
membership, `Model::get_ctr_by_index` retrieves a constraint by its current
model index, and `Model::remove` removes it. Indices may change when objects
are removed.

After optimization, `Model::get_ctr_dual` and `Model::get_ctr_farkas` expose
solver results when the attached optimizer and solution status provide them.
These values are optimizer capabilities; they are not available for every
model and every solve result.

\section lib_milp_basics_advanced_constraints_quadratic Quadratic Constraints

A quadratic constraint stores a `QuadExpr<Var>` and a `CtrType`. Its expression
is interpreted relative to zero. For example, the following adds
\\( x^2 + y \le 10 \\).

```cpp
const auto quadratic = model.add_qctr(
    x * x + y - 10,
    LessOrEqual,
    "quadratic"
);
```

The corresponding model accessors are `Model::qctrs()`,
`Model::get_qctr_expr`, `Model::get_qctr_type`,
`Model::get_qctr_index`, and `Model::get_qctr_by_index`. Membership and removal
use the same `Model::has` and `Model::remove` overloads as other optimization
objects.

```cpp
const auto& expression = model.get_qctr_expr(quadratic);
const auto quadratic_type = model.get_qctr_type(quadratic);

if (model.has(quadratic)) {
    model.remove(quadratic);
}
```

The current `Model` API does not expose setters for the expression or type of
an existing quadratic constraint. Remove and recreate the constraint when a
different version is needed. Solver support for quadratic constraints also
depends on the selected optimizer.

\section lib_milp_basics_advanced_constraints_sos SOS Constraints

An SOS constraint contains variables \\( x_j \\) with associated weights and is
either SOS1 or SOS2. In an SOS1 constraint, at most one \\( x_j \\) may be
nonzero; in an SOS2 constraint, at most two adjacent variables in weight order
may be nonzero.
The model-level constructor takes a Boolean whose value is `true` for SOS1 and
`false` for SOS2.

```cpp
const auto lambda = model.add_vars(
    Dim<1>(3), 0, 1, Continuous, 0, "lambda"
);

const auto sos1 = model.add_sosctr(
    true,
    { lambda[0], lambda[1], lambda[2] },
    { 1, 2, 3 },
    "choose_one"
);
```

An SOS constraint must contain at least two variables, all variables must
already belong to the model, and the variable and weight vectors must have
the same length. The methods `Model::is_sos1`, `Model::get_sosctr_vars`, and
`Model::get_sosctr_weights` access its model-specific data. SOS constraints
can be iterated with `Model::sosctrs()` and removed with `Model::remove`.
Support for SOS constraints is optimizer-specific.

\page lib_milp_basics_advanced_objectives The Objective Function
\brief Describes how to define, inspect, and modify a model's objective function.
\tableofcontents

The objective function tells an optimizer which quantity to minimize or
maximize. In `idol`, the objective expression is stored as a
`QuadExpr<Var>`, so the same interface accepts linear, affine, and quadratic
objectives. A new `Model` has the zero objective and uses the `Minimize` sense.

\section lib_milp_basics_advanced_objectives_set Setting the Objective

Use `Model::set_obj_expr` to replace the complete objective expression. The
following example sets the affine objective \\( 2x+3y+5 \\).

```cpp
Env env;
Model model(env);

const auto x = model.add_var(0, Inf, Continuous, 0, "x");
const auto y = model.add_var(0, Inf, Continuous, 0, "y");

model.set_obj_expr(2 * x + 3 * y + 5);
```

Build the expression from variables that already belong to the model. Calling
`set_obj_expr` replaces the old linear, quadratic, and constant terms; it does
not add to the existing objective.

A quadratic objective, such as
\\( x^2+2xy+y^2-4x \\), is set through the same method.

```cpp
model.set_obj_expr(x * x + 2 * x * y + y * y - 4 * x);
```

Whether a particular objective can be solved depends on the attached
optimizer. The model can represent a quadratic objective even when a selected
solver does not support it.

\section lib_milp_basics_advanced_objectives_sense Objective Sense

The `ObjectiveSense` values are `Minimize` and `Maximize`. Change the sense
with `Model::set_obj_sense` and query it with `Model::get_obj_sense`.

```cpp
model.set_obj_sense(Maximize);

if (model.get_obj_sense() == Maximize) {
    std::cout << "The model is a maximization problem." << std::endl;
}
```

The sense is independent of the sign of the expression. Changing it updates
an attached optimizer through its objective-sense update mechanism.

\section lib_milp_basics_advanced_objectives_access Accessing and Modifying the Objective

`Model::get_obj_expr` returns the complete quadratic expression. Its
`affine()` accessor gives the affine part, from which the linear terms and
constant can be inspected.

```cpp
const auto& objective = model.get_obj_expr();
const double constant = objective.affine().constant();
const double coefficient = model.get_var_obj(x);

std::cout << "constant: " << constant << std::endl;
std::cout << "coefficient of x: " << coefficient << std::endl;
```

For a focused update, `Model::set_var_obj` changes one coefficient in the
linear part and `Model::set_obj_const` changes the constant term.

```cpp
model.set_var_obj(x, -2);
model.set_obj_const(10);
```

These methods preserve the other objective terms. To modify quadratic terms,
construct a new expression and pass it to `Model::set_obj_expr`.

After a solve, `Model::get_best_obj` returns the objective value of the best
known feasible solution and `Model::get_best_bound` returns the optimizer's
best bound. Query the solution status first, because the availability and
meaning of result values depend on the solve outcome and optimizer.

\page lib_milp_basics_advanced_optimizers Optimizers and Optimizer Factories
\brief Explains how optimizer factories configure and create model-bound optimizers.
\tableofcontents

Formulating a `Model` and solving it are separate operations in `idol`. A model
contains variables, constraints, and an objective, but it can only be solved
after an optimizer has been attached.

Two related classes implement this separation.

- An `OptimizerFactory` is a reusable configuration object. It stores the
  choices needed to construct an optimizer, including common parameters and,
  for algorithmic factories, subordinate strategies or optimizers.
- An `Optimizer` is the runtime object bound to one `Model`. It synchronizes
  the model with a solver or algorithm, performs optimization, and owns the
  resulting status and solution interface.

Users normally configure factories and let `Model` create and own optimizers.

\section lib_milp_basics_advanced_optimizers_lifecycle Normal Lifecycle

The usual sequence is to create a model, configure a factory, pass the factory
to `Model::use`, and then optimize.

```cpp
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

using namespace idol;

Env env;
Model model(env);
const auto x = model.add_var(0, Inf, Continuous, -1, "x");
model.add_ctr(x <= 4);

auto highs = HiGHS();
highs.with_time_limit(60).with_logs(true);

model.use(highs);
model.optimize();
```

`Model::use` asks the factory to create an optimizer for the model, builds that
optimizer, stores it, and clones the factory into the model. Consequently, the
local `highs` variable does not need to outlive the model. Calling
`Model::optimize` first synchronizes pending updates and then invokes the
optimizer's solve routine.

Calling `optimize` without an attached optimizer throws an exception.
`Model::has_optimizer` tests whether one is attached, and `Model::unuse`
removes both the optimizer and the stored factory. Calling `use` again replaces
the currently attached optimizer.

\section lib_milp_basics_advanced_optimizers_parameters Configuring Factories

Factories derived from `OptimizerFactoryWithDefaultParameters` provide common
`with_*` methods. For example:

```cpp
auto optimizer = HiGHS()
    .with_time_limit(300)
    .with_thread_limit(4)
    .with_tol_mip_relative_gap(1e-4)
    .with_presolve(true)
    .with_logs(true);

model.use(optimizer);
```

Values explicitly set on the factory take precedence over the corresponding
defaults stored in the model's `Env`. A particular optimizer may reject or
ignore capabilities that its underlying solver does not provide; consult that
optimizer's interface for solver-specific options.

\section lib_milp_basics_advanced_optimizers_results Querying Results

The model forwards the common result interface to its attached optimizer.

```cpp
model.optimize();

const auto status = model.get_status();
std::cout << "Status: " << status << std::endl;
std::cout << "Reason: " << model.get_reason() << std::endl;

if (status == Optimal || status == Feasible || status == SubOptimal) {
    std::cout << "Objective: " << model.get_best_obj() << std::endl;
    std::cout << "x = " << model.get_var_primal(x) << std::endl;
}
```

Other common queries include `Model::get_best_bound`, solution-pool access
through `Model::get_n_solutions` and `Model::set_solution_index`, reduced
costs, rays, dual values, and Farkas certificates. Their availability depends
on the status and on the concrete optimizer.

\section lib_milp_basics_advanced_optimizers_access Accessing the Runtime Optimizer

Advanced code can obtain the model-owned runtime object through
`Model::optimizer`. The returned type is `Optimizer&`. Use `Optimizer::is<T>`
to test its concrete type and `Optimizer::as<T>` to obtain a checked reference.

```cpp
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/Optimizers_HiGHS.h"

if (model.optimizer().is<Optimizers::HiGHS>()) {
    const auto& highs = model.optimizer().as<Optimizers::HiGHS>();
    std::cout << "Optimizer: " << highs.name() << std::endl;
}
```

The runtime type is `Optimizers::HiGHS`, whereas the factory type is `HiGHS`.
Trying to cast to the factory type, or to any other incorrect runtime type,
throws an exception. Prefer the `Model` result methods unless an
optimizer-specific feature is actually needed.

\section lib_milp_basics_advanced_optimizers_composition Composing Algorithms

Many algorithmic factories accept other optimizer factories for subproblems.
This makes solver choice part of configuration rather than hard-coding it in
the algorithm. For example, the current branch-and-bound factory accepts a
node optimizer together with branching and node-selection rules.

```cpp
auto branch_and_bound = BranchAndBound<DefaultNodeInfo>()
    .with_node_optimizer(HiGHS::ContinuousRelaxation())
    .with_branching_rule(MostInfeasible())
    .with_node_selection_rule(BestBound())
    .with_time_limit(300);

model.use(branch_and_bound);
```

Each component remains a factory or strategy object until the owning
algorithm creates the runtime objects it needs. Composition is specific to
the public configuration methods of each algorithm; it does not imply that
arbitrary factories can be combined through a universal interface.

\page lib_milp_basics_advanced_create_optimizer Creating Your Own Optimizer
\brief Introduces the existing extension points for implementing an optimizer.
\tableofcontents

Implementing a new optimizer requires two cooperating classes: a public
factory derived from `OptimizerFactory`, and a model-bound runtime class
derived from `Optimizer` or one of its helper classes. The factory captures
configuration. The runtime class implements the solver or algorithm and reads
the model through `Optimizer::parent()`.

This page describes the existing architecture. It is not a complete solver
wrapper implementation; the details of translating objects and retrieving
solutions necessarily depend on the external solver or algorithm.

\section lib_milp_basics_advanced_create_optimizer_factory The Factory

A factory has two essential protected/public operations.

- `create(const Model&) const` allocates and returns a new runtime
  `Optimizer` bound to the supplied model.
- `clone() const` returns an independent copy of the factory. `Model::use`
  stores this copy after creating the runtime optimizer.

Most factories derive from `OptimizerFactoryWithDefaultParameters<Factory>`
instead of directly from `OptimizerFactory`. The CRTP helper provides the
standard fluent methods such as `with_time_limit`, `with_logs`, and tolerance
configuration. A custom factory can add its own `with_*` methods and retain
their values until `create` is called.

The current `HiGHS` factory is a compact representative pattern: it stores the
continuous-relaxation option, creates an `Optimizers::HiGHS` with the model and
that option, and implements `clone` by copying itself. The base
`OptimizerFactory::operator()` then applies factory values or environment
defaults to the newly created optimizer.

Factory configuration objects should own or clone any subordinate factories
needed by an algorithm. Do not store references to temporary configuration
objects whose lifetimes end before `create` uses them.

\section lib_milp_basics_advanced_create_optimizer_runtime The Runtime Optimizer

Every runtime optimizer is constructed with `const Model&` and passes it to
the `Optimizer` constructor. `Optimizer::parent()` then provides the
model-specific rows, columns, bounds, objective, and object indices.

The abstract `Optimizer` interface groups the responsibilities of a concrete
implementation as follows.

| Responsibility | Core virtual methods |
|----------------|----------------------|
| Initial construction | `build()` |
| Solve lifecycle | `hook_optimize()`; optionally `hook_before_optimize()` and `hook_after_optimize()` |
| Object changes | `add(...)`, `remove(...)`, and `update()` |
| Attribute changes | `update_obj_sense`, `update_obj`, `update_rhs`, `update_obj_constant`, matrix/constraint/variable update methods |
| Common results | `get_status`, `get_reason`, `get_best_obj`, `get_best_bound`, primal/dual/ray/Farkas and gap methods |
| Solution pools | `get_n_solutions`, `get_solution_index`, and `set_solution_index` |
| Utilities | `name()` and `write(...)` |

The overloads for adding and removing `Var`, `Ctr`, and `QCtr` are mandatory.
The base implementation of SOS add/remove throws an unsupported-feature
exception, so an optimizer only overrides those operations when it supports
SOS constraints. Likewise, a valid implementation may throw a clear
exception from a result method or update path that it does not support; it
must not return invented data.

`Optimizer::optimize()` itself is managed by the base class. It calls
`update()`, starts the timer, invokes the before/solve/after hooks, and stops
the timer. Concrete implementations put the actual solve in
`hook_optimize()` rather than replacing that lifecycle.

\section lib_milp_basics_advanced_create_optimizer_patterns Implementation Patterns

There are two common patterns in the current codebase.

**Solver wrappers.** A wrapper such as `Optimizers::HiGHS` derives from
`OptimizerWithLazyUpdates`. It maps idol objects to solver-side handles,
translates pending additions and changes in `hook_add` and `hook_update`, calls
the external solver in `hook_optimize`, and translates solver statuses and
solutions back to idol's common interface.

**Algorithmic optimizers.** A native algorithm may derive from `Optimizer`
directly or use `Algorithm`, which already stores the common status, reason,
best objective, and best bound and implements the gap calculations. Such an
optimizer may construct internal models and attach subordinate optimizer
factories to solve them. It still must define how changes to its parent model
affect internal state.

Choose the pattern that matches the implementation. A solver wrapper needs
incremental synchronization and solver-handle mappings; an algorithm that
rebuilds an internal formulation may instead invalidate that formulation when
the parent changes.

\section lib_milp_basics_advanced_create_optimizer_lazy Lazy Update Support

`OptimizerWithLazyUpdates<VarImplT, CtrImplT, QCtrImplT, SOSCtrImplT>` is a
helper for wrappers that want to defer synchronization until `update`,
`optimize`, or `write` requires it. The template arguments are the solver-side
handle types associated with idol variables and constraints.

Derived classes implement hooks such as `hook_build`, `hook_add`,
`hook_update`, `hook_remove`, `hook_update_objective`,
`hook_update_objective_sense`, `hook_update_rhs`, and `hook_update_matrix`.
The helper tracks pending objects and exposes `operator[]` for accessing the
stored variable and linear-constraint handles.

The current helper has explicit capability boundaries: updating an existing
quadratic or SOS constraint is not supported by its lazy-update path and
throws an exception. A new optimizer should document its own supported object
types and update operations rather than implying that the base interface
guarantees solver support.

\section lib_milp_basics_advanced_create_optimizer_checklist Practical Checklist

Before exposing a new optimizer factory, verify that the implementation:

1. creates and clones the factory without lifetime leaks;
2. binds each runtime optimizer to exactly one parent model;
3. translates every supported model object and objective sense correctly;
4. handles additions, removals, and attribute changes consistently;
5. reports unsupported quadratic or SOS features explicitly;
6. maps statuses, reasons, bounds, solutions, rays, and certificates without
   manufacturing unavailable values;
7. applies common parameters and tolerances supported by the backend;
8. includes tests for initial construction, re-optimization after updates,
   result access, and unsupported operations.

Representative implementations to study are the `HiGHS` factory and
`Optimizers::HiGHS` runtime wrapper for solver integration, and
`LambdaOptimizer` with `Optimizers::LambdaOptimizer` for a small algorithmic
optimizer using the `Algorithm` base class.
