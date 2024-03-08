.. _modeling_optimization_problems:

.. role:: cpp(code)
   :language: cpp

Modeling
========

This page introduces the basic concepts of modeling optimization problems in idol.
We will discuss how to create optimization models and add variables, constraints and objective functions to that model.
We will also see how to access the different components of an existing model.


All classes which are used for modeling (standard) optimization problems can be accessed by including :code:`#include <idol/modeling.h>`.

To simplify things, we will use the :code:`namespace idol` in the following examples. This dispenses us from prefixing
every class with :code:`idol::`. For instance, we will use :code:`Model` instead of :code:`idol::Model`.

Let's get started.

.. toctree::
    :maxdepth: 1
    :glob:

    environment
    models
    variables
    expressions
    constraints
    objective-function
