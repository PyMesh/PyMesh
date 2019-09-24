Mesh Generation
===============

Triangulation
-------------

Triangulation in 2D is often solved using Shewchuk's `triangle library
<https://www.cs.cmu.edu/~quake/triangle.html>`_.  It is both robust and
flexible.  We provide a pythonic wrapper over Shewchuk's triangle that exposes
most of its powers.

.. autoclass:: pymesh.triangle

Tetrahedralization
------------------

In contrast with 2D, tetrahedralization in 3D is a much harder problem.  Many
algorithms try to tackle this problem from different angles.  No single
algorithm or package stands out as the best.  We therefore offer a number of
different engines for our users.

.. autofunction:: pymesh.tetrahedralize

In addition to :py:func:`pymesh.tetraheralize`, we also provide a more complete
wrapper around Si's awesome `TetGen <http://wias-berlin.de/software/tetgen/>`_
package.

.. autoclass:: pymesh.tetgen
