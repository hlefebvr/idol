.. _api_Callback:

Callback
========

Callbacks are user-defined functions which are invoked during the execution of branch-and-bound algorithms at certain
key steps of the procedure. These key points are called :ref:`events <api_CallbackEvent>` in idol.
Callback functions can be used to monitor the algorithm's progress, control its behavior, or perform additional processing.

UserCutCallback and LazyCutCallback
-----------------------------------

A common use of callbacks is to implement custom cut generation procedures at each node of the branch-and-bound tree. Note that
idol offers out-of-the-box callback routines for cut separation. Namely, :ref:`UserCutCallback <api_UserCutCallback>`
and :ref:`LazyCutCallback <api_LazyCutCallback>`. Yet, it is totally possible to define your own cut generation procedure
by using the ``Callback::add_user_cut`` and  ``Callback::add_lazy_cut`` methods inside your callback.

In a nutshell, :ref:`UserCutCallback <api_UserCutCallback>` (or, similarly, ``Callback::add_user_cut``) are intended for
cuts which have a potential to improve the quality of the relaxation but whose presence in the model is not necessary for the
branch-and-bound to converge to an optimal solution. :ref:`UserCutCallback <api_UserCutCallback>` callbacks are called when
a non-valid solution has been found.

:ref:`LazyCutCallback <api_LazyCutCallback>` (or, similarly, ``Callback::add_lazy_cut``),
instead, are intended to generate constraints which should be part of the model but which were omitted in the model for some reason.
A typical reason for omitting constraints in a model's definition is the large number of such constraints.
:ref:`LazyCutCallback <api_LazyCutCallback>` callbacks are called when a valid solution has been found.

Creating your own callback
--------------------------

If you want to create your own callback, you should first create a new class which inherits the ``Callback`` class and
overrides the ``operator()(CallbackEvent)`` method.

Note that callbacks cannot be given "as-is" to an optimizer but must be passed through a ``CallbackFactory``. A callback
factory is a class whose role is to create a new callback object whenever it is needed. Every callback factories must be
a child (or little child) of the ``CallbackFactory`` class.

.. admonition:: Example

    We now give an example of callback which prints out every valid solution found.

    .. code-block::

        class MyCallback : public CallbackFactory {
        public:

            class Strategy { // Real callback implementation
            protected:
                void operator()(CallbackEvent t_event) {

                       if (t_event != IncumbentSolution) {
                            return;
                       }

                       std::cout << primal_solution() << std::endl;
                }
            }

            Callback* operator()() { // Creates a new callback object
                return new Strategy();
            }

            CallbackFactory* clone() const { // Creates a copy of the callback factory
                return new MyCallback(*this);
            }

        }

    Later, it is then possible to use this callback in compliant optimizers like :ref:`Gurobi <api_Gurobi>` or :ref:`BranchAndBound <api_BranchAndBound>`.

    .. code-block::

        model.use(
            Gurobi().with_callback(MyCallback();
        );

        model.optimize();

.. hint::

    Note that there also exists more advanced callbacks which are specific to idol's branch-and-bound implementation.
    For these advanced feature, see :ref:`BranchAndBoundCallback <api_BranchAndBoundCallback>`.

.. doxygenclass:: idol::Callback
    :protected-members:
