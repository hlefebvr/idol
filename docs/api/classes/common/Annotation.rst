Annotation
==========

Annotations are additional information associated to an optimization object (e.g., a constraint or a variable).
The Annotation class takes two template arguments: an optimization object type and a value type. For instance, we
can create an annotation for constraints of type :code:`unsigned int` as follows.

.. code:: cpp

    Env env;

    const unsigned int default_value = 0;

    Annotation<Ctr, unsigned int> annotation(env, "my_annotation", default_value);

Note that annotations are global, i.e., they do not relate to a given optimization model and can, therefore, be accessed
anywhere in the code. Thus, given a constraint called :code:`constraint`, the value of the annotation can be accessed
by calling the :code:`Ctr::get` method.

.. code:: cpp

    std::cout << "My annotation is " << constraint.get(annotation) << std::endl; // "0", i.e., the default_value value

Annotations are, in particular, used to give decomposition instructions to idol when designing a Branch-and-Price algorithm.
To learn more, check our `Branch-and-Price tutorials <tutorials_dantzig_wolfe>`_.

.. doxygenclass:: idol::Annotation
