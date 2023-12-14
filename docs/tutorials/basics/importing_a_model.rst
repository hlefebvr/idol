.. _importing_a_model:

.. role:: cpp(code)
   :language: cpp

Importing A Model
=================

In this tutorial, we will see how to import and export an optimization model from and to a file.
This can be useful if you want to read other people's instances of some optimization problem. For instance,
you may want to read instances from the `MIPLIB2017 <https://miplib.zib.de/>_` library.

Reading a Model
---------------

Reading a model from a file is implemented in idol through external solvers API. In short, we let external solvers
read and parse the optimization problem from a file and then convert back their representation into idol. This has
the advantage to perfectly adapt to each solver's convention in file formats (some standards are, indeed, ambiguous; e.g., MPS files).

Not all external solvers have this functionality implemented. As of now, the list of solvers which implement this are
Gurobi and GLPK.

Now, say you have an instance file stored at :code:`/path/to/my/instance.lp` in LP format. You may read this file by calling
the :code:`read_from_file` static method of the external solver acting as parsing backend. For instance, to use Gurobi,
one would to the following.

.. code::

    Env& env;
    auto model = Gurobi::read_from_file(env, "/path/to/my/instance.lp");

Notice that we gave an environment object as argument in addition to the path to the instance. This environment will be
used to create the model in idol.

Similarly, to use GLPK as parsing backend, the following is possible.

.. code::

    Env& env;
    auto model = GLPK::read_from_file(env, "/path/to/my/instance.lp");

Writing a Model
---------------

To write an optimization model to a file, the easiest way is again to rely on an external solver.
This is done by attaching a solver to a model, then by calling the :code:`Model::write` method.

.. code::

    Env env;
    Model model = some_function_creating_my_model(env); // this is assumed to create the desired optimization model

    model.use(Gurobi()); // We will use Gurobi as the optimizer

    model.write("instance.lp"); // Creates a new file "instance.lp" storing the model

Conclusion
----------

That's it! You are now capable of reading and writing optimization problems from and to a file! Congratulations!
Make sure to check out our other tutorials to learn more on how to use idol.
