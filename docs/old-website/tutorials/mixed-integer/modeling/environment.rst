The Environment
---------------

Every optimization object (such as variables and constraints) are managed by an "optimization environment".
It is the environment that controls the death and life of such objects. It is also through the environment that idol
manages the different versions each optimization object may have during the execution of your program.

Typically, only one environment should be used by your code. Yet, it is not forbidden to have several environments at the same time (not advised).

Environments are objects of the :ref:`Env <api_Env>` class, and can be created as follows.

.. code-block::

    Env env; // Creates a new optimization environment.
