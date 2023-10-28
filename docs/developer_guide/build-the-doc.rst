.. highlight:: sh
.. _dev_build_the_doc:

Build the doc!
==============

This page explains how to build the documentation on a local machine.

Dependencies
------------

Description
^^^^^^^^^^^

The documentation is built using `doxygen`_ and a bunch of other tools, in particular:

* `doxygen`_ the C++ documentation generator;
* `Sphinx`_ the python documentation generator;
* `sphinx-rtd-theme <https://github.com/readthedocs/sphinx_rtd_theme>`_ a Sphinx theme used to render the
  HTML version of the documentation;
* `sphinx-sitemap <https://sphinx-sitemap.readthedocs.io/en/latest/index.html>`_ a Sphinx extension
  to generate multi-version and multi-language sitemaps.org compliant sitemaps for the HTML version
  of a Sphinx documentation;
* `sphinx-copybutton <https://sphinx-copybutton.readthedocs.io/en/latest/>`_ a Sphinx extension which
  automatically adds buttons on code-blocks environments to copy its content;
* `Breathe`_ the Sphinx plugin for integrating doxygen outputs into Sphinx.

For a detailed description on how these tools interact, please refer to `this article <https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/#post-24393-footnote-ref-2>`_.

Installation guide (Ubuntu)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    sudo apt-get install doxygen # installs the last version of doxygen

    sudo apt-get install python3-sphinx # installs the last version of Sphinx

    sudo apt-get install python3-breathe # installs the last version of Breathe

    pip3 install sphinx-sitemap sphinx-rtd-theme sphinx-copybutton # installs theme and extensions

Build instructions
------------------

By default, CMake automatically creates a ``doc`` target. All you have to do to build the documentation website
is to build this target. For instance, inside a ``build`` directory, the following command can be used.

::

    cmake .. && make doc

Note that you do not need to use ``make`` and use the following generic command.

::

    cmake --build . --target doc

.. _doxygen: https://doxygen.nl/
.. _Sphinx: https://www.sphinx-doc.org/en/master/
.. _Breathe: https://github.com/breathe-doc/breathe