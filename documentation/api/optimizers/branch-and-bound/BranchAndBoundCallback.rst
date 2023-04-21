.. _api_BranchAndBoundCallback:

BranchAndBoundCallback
======================

This class can be used to create callbacks to run on idol's :ref:`branch-and-bound implementation <api_BranchAndBound>`.

.. warning::

    BranchAndBoundCallback-s is an *advanced* feature.

    Please, make sure that what you are trying do cannot be done with
    a :ref:`solver-independent callback <api_Callback>` first, or with pre-existing callbacks, e.g.,
    :ref:`UserCutCallback <api_UserCutCallback>` or :ref:`LazyCutCallback <api_LazyCutCallback>`.

    The advantage of using a :ref:`BranchAndBoundCallback <api_BranchAndBoundCallback>` instead of a standard
    solver-independent :ref:`Callback <api_Callback>` lies in the possibility to access specific information regarding
    the execution of the branch-and-bound algorithm. For instance, accessing a node's information, or the current
    relaxation model being solved.

.. admonition:: Example

    Here is an example of callback which prints out the event triggering it and, when the event corresponds to `NodeLoaded`,
    prints the node's model to be solved.

    .. code-block::

        class MyCallback : public BranchAndBoundCallbackFactory<NodeInfo> {
        public:
            class Strategy : public BranchAndBoundCallback<NodeInfo> {
            protected:
                void operator()(CallbackEvent t_event) override {

                    std::cout << "MyCallback is called for " << t_event << std::endl;

                    if (t_event != NodeLoaded) {
                        return;
                    }

                    std::cout << "The problem being solve at node " << node().id() << std::endl;
                    std::cout << relaxation() << std::endl;

                }
            };

            BranchAndBoundCallback<NodeInfo> *operator()() override {
                return new Strategy();
            }

            [[nodiscard]] BranchAndBoundCallbackFactory<NodeInfo> *clone() const override {
                return new MyCallback(*this);
            }
        };

    Then, this callback can be used as follows.

    .. code-block::

        model.use(
            BranchAndBound()
                .with_node_optimizer(GLPK::ContinuousRelaxation())
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(DepthFirst())
                .with_callback(MyCallback());
        );

.. hint::

    By default, most of the objects returned by BranchAndBoundCallback methods are const. If you wish to access non-const
    versions (e.g., if you want to perform non-standard updates to the relaxed model or want to change the node's current
    information manually), you should use the advanced interface obtained by calling ``BranchAndBoundCallback::advanced_interface``.

.. doxygenclass:: BranchAndBoundCallback
    :protected-members:
