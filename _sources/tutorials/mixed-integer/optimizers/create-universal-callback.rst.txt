.. _tutorial_create_callback:

Creating Your Own Callback
==========================

In this page, we see how to create your own universal callback.
Universal callbacks are callbacks which are not specific to a particular optimizer.
In that sense, they are generic and can be used with different optimizers without any modification.

Creating your own callback can useful if you want to create a callback that is not included in the
default set of callbacks implemented in idol.
If you are looking for quick and easy-to-use callbacks for separating user cuts or lazy constraints,
you can refer to :ref:`this page <tutorial_user_lazy_cuts>`.

.. hint::

    Note that there are also callbacks specific to the :code:`BranchAndBound` optimizer.
    If you are looking for this, you can refer to :ref:`this page <tutorial_create_bb_callback>`.

.. contents:: Table of Contents
    :local:
    :depth: 2

The Basics
^^^^^^^^^^

Creating your callback is done by creating a sub-class of the :ref:`Callback <api_callback>` class and overriding the
:code:`Callack::operator()` method. It is this method that will be called by the solver whenever an incumbent is found.

Note, however, that callbacks cannot be given "as-is" to an optimizer but must be passed through a :ref:`CallbackFactory <api_CallbackFactory>`. A callback
factory is a class whose role is to create a new callback object whenever it is needed. Every callback factories must be
a child (or little child) of the :ref:`CallbackFactory <api_CallbackFactory>` class.

The following code shows how to create a callback factory and a callback that prints the value of the primal solution at each
iteration.

.. code:: cpp

    /* Callback factory implementation */
    class MyCallback : public CallbackFactory {
    public:

        /* Actual callback implementation */
        class Strategy {
        protected:

            /* This method is called whenever an event occurs
               during the solution process */
            void operator()(CallbackEvent t_event) {

                   if (t_event != IncumbentSolution) {
                        return;
                   }

                   std::cout << primal_solution() << std::endl;
            }

        }

        /* This method creates a new callback object */
        Callback* operator()() {
            return new Strategy();
        }

        /* This method creates a copy of the callback factory */
        CallbackFactory* clone() const {
            return new MyCallback(*this);
        }

    }

As you can see, the callback factory has two important methods: :code:`operator()` and :code:`clone()`. The :code:`operator()`
method is used to create a new callback object, while the :code:`clone()` method is used to create a copy of the callback factory.

The nested class :code:`Strategy` is the actual callback implementation. It is a sub-class of the :ref:`Callback <api_callback>` class and
overrides the :code:`operator()` method. In this example, the callback prints the value of the primal solution whenever the event
triggering the callback is :code:`IncumbentSolution`. In other words, this callback will print out all incumbent solutions found by the solver.

As for the :ref:`UserCutCallback <api_UserCutCallback>` and :ref:`LazyConstraintCallback <api_LazyCutCallback>` classes,
our new callback can be added to an optimizer as follows.

.. code:: cpp

    model.use(
        Gurobi().add_callback(MyCallback())
    );

    model.optimize();

List of Optimizers Supporting Universal Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following solvers support universal callbacks:

- :code:`Gurobi`
- :code:`BranchAndBound`
- :code:`Mosek`

Hence, for these solvers, you can create your own callback and add it to the optimizer without having to
worry about the underlying solver.

An Example: Knapsack Cover Cuts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. hint::

    This section is dedicated to the "advanced topic" of knapsack cover inequalities.
    Rudimentary notions on `Knapsack problems <https://en.wikipedia.org/wiki/Knapsack_problem>`_ and
    `Cover inequalities <https://www.lancaster.ac.uk/staff/letchfoa/other-publications/2010-EORMS-cover-ineqs.pdf>`_ are recommended.


In this example, we will show how to create a callback that separates knapsack cover cuts. A knapsack cover cut is a valid inequality
for the knapsack problem. It is defined as follows:

.. math::

    \sum_{i \in C} x_i \leq |C| - 1


where :math:`C` defines a cover of the knapsack, i.e., a set of items such that the sum of their weights is greater than the capacity of the knapsack.

Given a solution :math:`\hat x` to the continuous relaxation of the knapsack problem, we can check whether it violates a cover inequality.
This is done by solving the following separation problem.

.. math::

    \begin{align}
        \max_{z} \quad & (1 - \hat x)^\top z \qquad \ge 1 \\
        \text{s.t.} \quad & w^\top z \ge W + 1, \\
        & z\in\{0,1\}^n.
    \end{align}

A cover inequality is violated if and only if the optimal objective value of this problem is strictly less than 1.
In such a case, a new cut should be added.

We will write a callback that separates knapsack cover cuts.

To this end, we first create our knapsack problem model. This is done as follows.

.. code:: cpp

    Env env;
    Model knapsack(env, Maximize);

    const auto x = knapsack.add_vars(Dim<1>(n), 0, 1, Binary, 0, "x");

    knapsack.add_ctr(idol_Sum(i, Range(n_items), w[i] * x[i]) <= W);
    knapsack.set_obj_expr(idol_Sum(i, Range(n_items), p[i] * x[i]));

Then, we create our callback factory. It is this factory that will be used to create a new callback object when needed.
Since we need to pass some parameters to the callback, we will use the constructor of the callback factory to pass these parameters.
This is done as follows.

.. code:: cpp


    class KnapsackCover : public CallbackFactory {
        const std::vector<Var> m_x;
        const std::vector<double> m_weights;
        const std::vector<double> m_profits;
        const double m_capacity;
    public:
        KnapsackCover(const std::vector<Var>& t_x,
                      const std::vector<double>& t_weights,
                      const std::vector<double>& t_profits,
                      double t_capacity)
                        : m_x(t_x), m_weights(t_weights),
                          m_profits(t_profits), m_capacity(t_capacity) {}

        class Strategy;

        Callback* operator()() {
            return new Strategy(m_x, m_weights, m_profits, m_capacity);
        }

        CallbackFactory* clone() const {
            return new MyCallback(*this);
        }

    }

The actual implementation of the callback is done in the nested class :code:`Strategy`.
This class is a sub-class of the :ref:`Callback <api_callback>` class and is defined as follows.

.. code:: cpp

    class KnapsackCover::Strategy {
        const std::vector<Var> m_x;
        const std::vector<double> m_weights;
        const std::vector<double> m_profits;
        const double m_capacity;
    protected:
        Strategy(const std::vector<Var>& t_x,
                 const std::vector<double>& t_weights,
                 const std::vector<double>& t_profits,
                 double t_capacity)
                    : m_x(t_x), m_weights(t_weights),
                      m_profits(t_profits), m_capacity(t_capacity) {}

        void operator()(CallbackEvent t_event) {

               if (t_event != InvalidSolution) {
                    return;
               }

               auto& env = parent().env();
               const auto fractional_point = primal_solution();

                Model separation(env, Maximize);

                const auto z = separation.add_vars(Dim<1>(m_x.size()), 0, 1, Binary, "z");
                separation.add_ctr(idol_Sum(i, Range(m_x.size()), m_weights[i] * z[i]) >= m_capacity + 1);
                separation.set_obj_expr(idol_Sum(i, Range(m_x.size()), (1 - fractional_point[i]) * z[i]));

                separation.use(Gurobi());

                separation.optimize();

                if (separation.get_best_obj() < 1) {
                    return;
                }

                const auto cut = idol_Sum(i, Range(m_x.size()), separation.get_var_primal(z[i]) * (1 - x[i])) >= 1;

                add_user_cut(cut);

        }
    }

Finally, we can add our callback to the optimizer as follows.

.. code::

    knapsack.use(
        Gurobi::Continuous().add_callback(KnapsackCover(x, w, p, W))
    );

    knapsack.optimize();
