Geometry Processing Functions
=============================


Boolean opertions
-----------------

.. autofunction:: pymesh.boolean

While all solid geometry operations can be done as a sequence of binary boolean
operations. It is beneficial sometimes to use :py:class:`pymesh.CSGTree` for
carrying out more complex operations.

.. autoclass:: pymesh.CSGTree
    :members:

Convex hull
-----------

.. autofunction:: pymesh.convex_hull

Outer hull
----------

.. autofunction:: pymesh.compute_outer_hull

Mesh arrangement
----------------

3D arrangement:

.. autofunction:: pymesh.partition_into_cells

2D arrangement:

.. autoclass:: pymesh.Arrangement2

Minkowski sum
-------------

.. autofunction:: pymesh.minkowski_sum

Subdivision
-----------

.. autofunction:: pymesh.subdivide

Winding number query
--------------------

.. autofunction:: pymesh.compute_winding_number

Slicing mesh
------------

.. autofunction:: pymesh.slice_mesh


Distance to mesh query
----------------------

.. autofunction:: pymesh.distance_to_mesh

