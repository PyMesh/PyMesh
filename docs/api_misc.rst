Miscellaneous functions
=======================

Matrix I/O
----------

In addition to `numpy.save` and `numpy.load`, we also provide an alternative way
of saving and loading `numpy.ndarray` to files.

.. autofunction:: pymesh.save_matrix
.. autofunction:: pymesh.load_matrix

Mesh type conversion
--------------------

.. autofunction:: pymesh.quad_to_tri
.. autofunction:: pymesh.hex_to_tet

Attribute type conversion
-------------------------

.. autofunction:: pymesh.convert_to_vertex_attribute
.. autofunction:: pymesh.convert_to_vertex_attribute_from_name
.. autofunction:: pymesh.convert_to_face_attribute
.. autofunction:: pymesh.convert_to_face_attribute_from_name
.. autofunction:: pymesh.convert_to_voxel_attribute
.. autofunction:: pymesh.convert_to_voxel_attribute_from_name

Attribute mapping
-----------------

.. autofunction:: pymesh.map_vertex_attribute
.. autofunction:: pymesh.map_face_attribute
.. autofunction:: pymesh.map_corner_attribute

Quaternion
----------

.. autoclass:: pymesh.Quaternion
    :members:

Exact number types
------------------

.. autoclass:: pymesh.Gmpz
    :members:

.. autoclass:: pymesh.Gmpq
    :members:

Exact predicates
----------------

.. autofunction:: pymesh.orient_2D
.. autofunction:: pymesh.orient_3D
.. autofunction:: pymesh.in_circle
.. autofunction:: pymesh.in_sphere

The following predicates are built on top of above fundamental predicates.

.. autofunction:: pymesh.is_colinear
.. autofunction:: pymesh.get_degenerated_faces
.. autofunction:: pymesh.get_degenerated_faces_raw
.. autofunction:: pymesh.get_tet_orientations
.. autofunction:: pymesh.get_tet_orientations_raw
.. autofunction:: pymesh.is_delaunay_raw
.. autofunction:: pymesh.is_delaunay

Mesh compression
----------------

.. autofunction:: pymesh.compress
.. autofunction:: pymesh.decompress

Mesh to graph
-------------

.. autofunction:: pymesh.mesh_to_graph
.. autofunction:: pymesh.mesh_to_dual_graph
