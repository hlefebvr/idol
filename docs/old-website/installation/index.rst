.. _installation:

Installation
============

You are here on the installation guidelines. This page will guide through the installation process of idol.

There are two ways to install idol: a local installation (easy to use) and a global installation (for more advanced users).

Local Installation
------------------

:ref:`A local installation <installation_local>` lets CMake automatically download and install idol in a sub-folder of your CMake project. This is the easiest way to get started.

Global Installation
-------------------

:ref:`A global installation <basics_installation>` installs idol globally on your computer (e.g., in :code:`/usr/lib`).

.. toctree::
   :hidden:
   :maxdepth: 1
   :glob:

   local_installation
   global-installation/index
   options

Options (External Solvers, etc.)
--------------------------------

No matter which installation method you choose, you can customize the installation by setting a set of parameters accordingly.
This is particularly useful if you want to install idol with some external solver such as Gurobi.
Please refer to the :ref:`complete list of options <cmake_options>` for more information.

.. toctree::
   :hidden:
   :maxdepth: 1
   :glob:

   options
