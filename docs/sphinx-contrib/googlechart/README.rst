googlechart extension README
============================

This is a sphinx extension which render charts and graphs by using
`Google Chart <http://code.google.com/intl/ja/apis/chart/>`_ .

source:

.. code-block:: text

   .. piechart::

      dog: 100
      cat: 80
      rabbit: 40

rendered:

.. piechart::

   dog: 100
   cat: 80
   rabbit: 40

Setting
=======

You can see available package at `PyPI <http://pypi.python.org/pypi/sphinxcontrib-googlechart>`_.

You can get source code at http://bitbucket.org/birkenfeld/sphinx-contrib/

Install
-------

.. code-block:: bash

   $ easy_install sphinxcontrib-googlechart


Configure Sphinx
----------------

To enable this extension, add ``sphinxcontrib.googlechart`` module to extensions 
option at :file:`conf.py`. 

.. code-block:: python

   # Enabled extensions
   extensions = ['sphinxcontrib.googlechart']


Directive
=========

.. describe:: .. piechart::

   This directive insert a piechart into the generated document.
   piechart directive takes code block as source script.

   Examples::

      .. piechart::

         dog: 100
         cat: 80
         rabbit: 40

   .. piechart::

      dog: 100
      cat: 80
      rabbit: 40

   If you want to change colors of chart,
   write color parameters at source script::

      .. piechart::

         dog: 100
         dog.color: ff0000
         cat: 80
         cat.color: 00ff00
         rabbit: 40
         rabbit.color: 0000ff

   .. piechart::

      dog: 100
      dog.color: ff0000
      cat: 80
      cat.color: 00ff00
      rabbit: 40
      rabbit.color: 0000ff


.. describe:: .. piechart3d::

   This directive insert a 3D piechart into the generated document.
   piechart directive takes code block as source script.

   Examples::

      .. piechart3d::
         :size: 480x240

         dog: 100
         cat: 80
         rabbit: 40

   .. piechart3d::
      :size: 480x240

      dog: 100
      cat: 80
      rabbit: 40


.. describe:: .. linechart::

   This directive insert a linechart into the generated document.
   linechart directive takes code block as source script.

   Examples::

      .. linechart::

         bicycle: 15, 35, 20, 40
         car: 60, 75, 60, 30

   .. linechart::

      bicycle: 15, 35, 20, 40
      car: 60, 75, 60, 30

   If you want to change colors of chart,
   write color parameters at source script::

      .. linechart::

         bicycle: 15, 35, 20, 40
         bicycle.color: ff0000
         car: 60, 75, 60, 30
         car.color: 0000ff

   .. linechart::

      bicycle: 15, 35, 20, 40
      bicycle.color: ff0000
      car: 60, 75, 60, 30
      car.color: 0000ff

   If you want to append axis to chart,
   write axis and axis_label parameters at source script::

      .. linechart::

         bicycle: 15, 35, 20, 40
         bicycle.color: ff0000
         bicycle.axis: x
         bicycle.axis_label: slow, fast
         car: 60, 75, 60, 30
         car.color: 0000ff

   .. linechart::

      bicycle: 15, 35, 20, 40
      bicycle.color: ff0000
      bicycle.axis: x
      bicycle.axis_label: slow, fast
      car: 60, 75, 60, 30
      car.color: 0000ff

   If you want to change colors of chart,
   write color parameters at source script::

      .. linechart::

         bicycle: 15, 35, 20, 40
         bicycle.color: ff0000
         car: 60, 75, 60, 30
         car.color: 0000ff

   .. linechart::

      bicycle: 15, 35, 20, 40
      bicycle.color: ff0000
      car: 60, 75, 60, 30
      car.color: 0000ff


.. describe:: .. linechartxy::

   This directive insert a linechart into the generated document.
   linechart directive takes code block as source script.

   Examples::

      .. linechartxy::

         bicycle: (0, 15), (30, 35), (60, 20), (90, 40)
         car: (0, 60), (20, 75), (40, 60), (90, 30)


   .. linechartxy::

      bicycle: (0, 15), (30, 35), (60, 20), (90, 40)
      car: (0, 60), (20, 75), (40, 60), (90, 30)


.. describe:: .. holizontal_barchart::

   This directive insert a barchart into the generated document.
   barchart directive takes code block as source script.

   Examples::

      .. holizontal_barchart::

         bicycle: 15, 25, 20, 30
         bicycle.color: ff0000
         bicycle.axis: x
         bicycle.axis_label: slow, fast
         car: 40, 50, 60, 45
         car.color: 0000ff

   .. holizontal_barchart::

      bicycle: 15, 25, 20, 30
      bicycle.color: ff0000
      bicycle.axis: x
      bicycle.axis_label: slow, fast
      car: 40, 50, 60, 45
      car.color: 0000ff


.. describe:: .. vertical_barchart::

   This directive insert a barchart into the generated document.
   barchart directive takes code block as source script.

   Examples::

      .. vertical_barchart::

         bicycle: 15, 25, 20, 30
         bicycle.color: ff0000
         bicycle.axis: y
         bicycle.axis_label: slow, fast
         car: 40, 50, 60, 45
         car.color: 0000ff

   .. vertical_barchart::

      bicycle: 15, 25, 20, 30
      bicycle.color: ff0000
      bicycle.axis: y
      bicycle.axis_label: slow, fast
      car: 40, 50, 60, 45
      car.color: 0000ff


.. describe:: .. holizontal_bargraph::

   This directive insert a barchart into the generated document.
   barchart directive takes code block as source script.

   Examples::

      .. holizontal_bargraph::

         bicycle: 15, 25, 20, 30
         bicycle.color: ff0000
         bicycle.axis: x
         bicycle.axis_label: slow, fast
         car: 40, 50, 60, 45
         car.color: 0000ff

   .. holizontal_bargraph::

      bicycle: 15, 25, 20, 30
      bicycle.color: ff0000
      bicycle.axis: x
      bicycle.axis_label: slow, fast
      car: 40, 50, 60, 45
      car.color: 0000ff


.. describe:: .. vertical_bargraph::

   This directive insert a barchart into the generated document.
   barchart directive takes code block as source script.

   Examples::

      .. vertical_bargraph::

         bicycle: 15, 25, 20, 30
         bicycle.color: ff0000
         bicycle.axis: y
         bicycle.axis_label: slow, fast
         car: 40, 50, 60, 45
         car.color: 0000ff

   .. vertical_bargraph::

      bicycle: 15, 25, 20, 30
      bicycle.color: ff0000
      bicycle.axis: y
      bicycle.axis_label: slow, fast
      car: 40, 50, 60, 45
      car.color: 0000ff


.. describe:: .. venndiagram::

   This directive insert a venn diagrams into the generated document.
   venndiagram directive takes code block as source script.

   Examples::

      .. venndiagram::

         data: 100, 80, 40, 20, 20, 20, 10

   .. venndiagram::

      data: 100, 80, 40, 20, 20, 20, 10


.. describe:: .. plotchart::

   This directive insert a plotchart into the generated document.
   plotchart directive takes code block as source script.

   Examples::

      .. plotchart::

         data: (50, 60), (75, 20), (20, 30), (10, 70), (45, 10)

   .. plotchart::

      data: (50, 60), (75, 20), (20, 30), (10, 70), (45, 10)
      data.axis: x, y
      data.axis_label: (slow, fast), (low, high)


.. describe:: .. mapchart::

   This directive inserts a mapchart into the generated document.
   mapchart direcrtive takes code block as a source script.

   Examples::

      .. mapchart::

         data: CN, JP, KR
         color: ff0000, 00ff00, 0000ff

      .. mapchart::

         CN: "People's Republic of China"
         CN.color: ff0000
         JP: Japan
         JP.color: 00ff00
         KR: "Republic of Korea"
         KR.color: 0000ff

   .. mapchart::

      data: CN, JP, KR
      color: ff0000, 00ff00, 0000ff

   .. mapchart::

      CN: "People's Republic of China"
      CN.color: ff0000
      JP: Japan
      JP.color: 00ff00
      KR: "Republic of Korea"
      KR.color: 0000ff


Graphviz charts on Google Chart
===============================

You can use Google Chart for graphviz chart generation.
``sphinxcontrib.googlechart.graphviz`` module is compatible with ``sphinx.ext.graphviz`` .
This module does not depend to `graphviz` binaries.

If you want to use graphviz for Google Chart` ,
add ``sphinxcontrib.googlechart.graphviz`` module to extention option.

.. code-block:: python

   # Enabled extensions
   extensions = ['sphinxcontrib.googlechart', 'sphinxcontrib.googlechart.graphviz']

.. describe:: .. graphviz:: [filename]

   This directive insert a graphviz graph into the generated document.
   graphviz directive takes filename argument or code block as source script.

   Examples::

      .. graphviz:: external.dot

      .. graphviz::

         digraph {
           A -> B;
         }

   .. graphviz::

      digraph {
        A -> B;
      }

.. describe:: .. graph:: [graph_id]

   This directive insert a graphviz graph into the generated document.
   graph directive takes graph_id argument and code block as source script.

   Examples::

      .. graph::

         A -- B;

   .. graph::

      A -- B;

.. describe:: .. digraph:: [graph_id]

   This directive insert a graphviz digraph into the generated document.
   digraph directive takes graph_id argument and code block as source script.

   Examples::

      .. digraph::

         A -- B;

   .. digraph::

      A -> B;


Repository
==========

This code is hosted by Bitbucket.

  http://bitbucket.org/birkenfeld/sphinx-contrib/
