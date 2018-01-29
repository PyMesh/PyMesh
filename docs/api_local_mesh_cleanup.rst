Local Mesh Cleanup
==================

Meshes coming from the real world are rarely clean.  Artifacts such as
degeneracies, duplicate vertex/triangles and self-intersections are rampant (See
the ``about`` page in our `Thingi10K dataset
<https://ten-thousand-models.appspot.com/>`_). Unfortunately, many geometry
processing operations have strict and often unspecified requirements on the
cleanness of the input geometry. Here, we provide a number of handy routines to
facilitate the task of cleaning up a mesh.

Remove isolated vertices
------------------------

Isolated vertices are vertices not referred by any face or voxel.  They often
does not contribute to the geometry (except for the case of point cloud), and
thus can be safely removed.

.. autofunction:: pymesh.remove_isolated_vertices
.. autofunction:: pymesh.remove_isolated_vertices_raw

Remove duplicate vertices
-------------------------

Duplicate or near duplicate vertices are vertices with nearly same coordinates.
Two vertices can be considered as duplicates of each other if their Euclidean
distance is less than a tolerance (labeled `tol`).  Duplicate vertices can often
be merged into a single vertex.

.. autofunction:: pymesh.remove_duplicated_vertices
.. autofunction:: pymesh.remove_duplicated_vertices_raw

Collapse short edges
--------------------

Short edges are edges with length less than a user specified threshold.  Use the
following functions to collapse *all* short edges in the mesh.  The output mesh
guarantees to have no short edges.

.. autofunction:: pymesh.collapse_short_edges
.. autofunction:: pymesh.collapse_short_edges_raw

Split long edges
----------------

Long edges are sometimes undesirable as well.  Use the following functions to
split long edges into 2 or more shorter edges.  The output mesh guarantees to
have no edges longer than the user specified threshold.

.. autofunction:: pymesh.split_long_edges
.. autofunction:: pymesh.split_long_edges_raw

Remove duplicate faces
----------------------

Duplicate faces are faces consisting of the same vertices.  They are often not
desirable and may cause numerical problems.  Use the following functions to
remove them.

.. autofunction:: pymesh.remove_duplicated_faces
.. autofunction:: pymesh.remove_duplicated_faces_raw

Remove obtuse triangles
-----------------------

Obtuse triangles are often not desirable due to their geometric nature (e.g.
`circumcenter <http://mathworld.wolfram.com/Circumcenter.html>`_ of obtuse
triangles are outside of the triangle).  Each obtuse triangle can always be
split into 2 or more right or sharp triangles. They can be removed using the
following routines.

.. autofunction:: pymesh.remove_obtuse_triangles
.. autofunction:: pymesh.remove_obtuse_triangles_raw

Remove degenerate triangles
---------------------------

Degenerate triangles are triangles with collinear vertices.  They have zero area
and their normals are undefined.  They can be removed using the following
routines.

.. autofunction:: pymesh.remove_degenerated_triangles
.. autofunction:: pymesh.remove_degenerated_triangles_raw

Self-intersections
------------------

Self-intersections are often a problem in geometry processing.  They can be
detected and resolved with the following routines.

.. autofunction:: pymesh.detect_self_intersection
.. autofunction:: pymesh.resolve_self_intersection

Separate mesh into disconnected components
------------------------------------------

.. autofunction:: pymesh.separate_mesh

Merge multiple meshes
---------------------

.. autofunction:: pymesh.merge_meshes

Submesh extraction
------------------

Sometimes, it is useful to extract a local region from a more complex mesh for
futher examination.  :py:func:`pymesh.submesh` is designed for this task.

.. autofunction:: pymesh.submesh

