.. MATLAB Sphinx Documentation Test documentation master file, created by
   sphinx-quickstart on Wed Jan 15 11:38:03 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to MATLAB Sphinx Documentation Test's documentation!
============================================================

Contents:
---------

.. toctree::
   :maxdepth: 2

Tests:
------

.. _test-data:

Test Data
^^^^^^^^^
This is the test data module

.. automodule:: test_data

:mod:`test_data` is a really cool module

.. _MyHandleClass:

My Handle Class
+++++++++++++++
Does this work?

.. autoclass:: MyHandleClass
    :show-inheritance:
    :members:

.. _MyAbstractClass:

My Abstract Class
+++++++++++++++++
Does this work?

.. autoclass:: MyAbstractClass
    :show-inheritance:
    :members:

.. _myfun:

my function
+++++++++++
Do these work?

.. autofunction:: myfun

.. _MyClass:

My Class
++++++++
How about this?

.. autoclass:: MyClass
    :show-inheritance:
    :members:

.. _mymethod:

my method
~~~~~~~~~
How about this too?

.. automethod:: MyClass.mymethod

Ellipsis Properties
+++++++++++++++++++
Some edge cases that have ellipsis inside arrays.

.. autoclass:: EllipsisProperties
    :show-inheritance:
    :members:

Submodule
+++++++++
This is the test data module

.. automodule:: test_data.test_submodule

Ellipsis after equals
~~~~~~~~~~~~~~~~~~~~~
.. autofunction:: f_ellipsis_after_equals

No Arguments
~~~~~~~~~~~~
.. autofunction:: f_no_args

No Outputs
~~~~~~~~~~
.. autofunction:: f_no_outputs

Output with Ellipsis
~~~~~~~~~~~~~~~~~~~~
.. autofunction:: f_output_with_ellipsis

Output with no commas
~~~~~~~~~~~~~~~~~~~~~
.. autofunction:: f_output_without_commas

Super from different module
~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. autoclass:: super_from_diff_mod
    :members:
    :show-inheritance:

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

