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
- \subpage lib_milp_basics_advanced_expressions (TODO)
- \subpage lib_milp_basics_advanced_constraints (TODO)
- \subpage lib_milp_basics_advanced_objectives (TODO)
- \subpage lib_milp_basics_advanced_optimizers (TODO)
- \subpage lib_milp_basics_advanced_create_optimizer (TODO)

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
\tableofcontents

\warning This page is still under construction. Thank you for your understanding.

\page lib_milp_basics_advanced_constraints Constraints
\tableofcontents

\warning This page is still under construction. Thank you for your understanding.

\page lib_milp_basics_advanced_objectives The Objective Function
\tableofcontents

\warning This page is still under construction. Thank you for your understanding.

\page lib_milp_basics_advanced_optimizers Optimizers and Optimizer Factories
\tableofcontents

\warning This page is still under construction. Thank you for your understanding.

\page lib_milp_basics_advanced_create_optimizer Creating Your Own Optimizer
\tableofcontents

\warning This page is still under construction. Thank you for your understanding.
