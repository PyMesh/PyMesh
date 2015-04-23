User Guide
==========

:mod:`collapse_short_edges`:
----------------------------

The following illustrate a simple use case:

Initialize two triangles.  One of them has a short edge.

    >>> import numpy as np
    >>> from pymesh.meshutils import collapse_short_edges
    >>> V = np.array([
    ...     [ 0.0, 0.0, 0.001 ],
    ...     [ 0.0, 0.0, 0.0   ],
    ...     [ 1.0, 0.0, 0.0   ],
    ...     [ 0.0, 1.0, 0.0   ],
    ... ], dtype=float)
    >>> F = np.array([
    ...     [0, 1, 2], # edge (0, 1) is really short
    ...     [1, 2, 3],
    ... ], dtype=int)

Collapse all edges shorter than 0.5.

    >>> OV, OF, info = collapse_short_edges(V, F, 0.5)
    >>> print(OV)
    [[  1.00000000e+00   0.00000000e+00   0.00000000e+00]
     [  0.00000000e+00   1.00000000e+00   0.00000000e+00]
     [  0.00000000e+00   0.00000000e+00   5.00000000e-04]]
    >>> print(OF)
    [[2 0 1]]
    >>> print(info)
    {'num_edge_collapsed': 1L, 'source_face_index': array([1], dtype=int32)}

By default, an edge will collapse onto its mid-point.

Here is another example that preserves the vertices on the crease.
Because edge ``(1, 2)`` has dihedral angle 90 degrees, vertex ``1`` will
be used when collapsing short edge ``(0, 1)``.

    >>> OV, OF, __ = collapse_short_edges(V, F, 0.5,
    ...     preserve_feature=True)
    >>>
    >>> print(OV)
    [[ 1.  0.  0.]
     [ 0.  1.  0.]
     [ 0.  0.  0.]]
    >>> print(OF)
    [[2 0 1]]

:mod:`remove_duplicated_faces`:
-------------------------------

Initializing two duplicated faces with different orientation.

    >>> import numpy as np
    >>> from pymesh.meshutils import remove_duplicated_faces
    >>> V = np.array([
    ...     [ 0.0, 0.0, 0.0],
    ...     [ 1.0, 0.0, 0.0],
    ...     [ 0.0, 1.0, 0.0],
    ... ], dtype=float)
    >>> F = np.array([
    ...     [0, 1, 2],
    ...     [2, 1, 0],
    ... ], dtype=int)

The two faces form a fin so both are removed.
    >>> OV, OF = remove_duplicated_faces(V, F)
    >>> print(OV)
    [[ 0.  0.  0.]
     [ 1.  0.  0.]
     [ 0.  1.  0.]]
    >>> print(OF)
    []

Notice that vertices are left unchanged.  Now, modify faces to there is a
dominant orientation.

    >>> F = np.array([
    ...     [0, 1, 2],
    ...     [0, 1, 2],
    ...     [2, 1, 0],
    ... ], dtype=int)
    >>> __, OF = remove_duplicated_faces(V, F)
    >>> print(OF)
    [[0 1 2]]

:mod:`remove_duplicated_vertices`:
----------------------------------

Here is an example:

    >>> import numpy as np
    >>> from pymesh.meshutils import remove_duplicated_vertices
    >>> V = np.array([
    ...     [ 0.0, 0.0, 0.0], # duplicated
    ...     [ 0.0, 0.0, 0.0], # duplicated
    ...     [ 1.0, 0.0, 0.0],
    ...     [ 0.0, 1.0, 0.0],
    ... ], dtype=float)
    >>> F = np.array([
    ...     [0, 2, 3],
    ...     [1, 2, 3],
    ... ], dtype=int)
    >>>
    >>> OV, OF, info = remove_duplicated_vertices(V, F)
    >>>
    >>> print(OV)
    [[ 0.  0.  0.]
     [ 1.  0.  0.]
     [ 0.  1.  0.]]

Note that vertex indices are updated. Also note that it could introduce
duplicated faces. See pymesh.meshutils.remove_duplicated_faces

    >>> print(OF)
    [[0 1 2]
     [0 1 2]]
    >>>
    >>> print(info)
    {'num_vertex_merged': 1L, 'index_map': array([0, 0, 1, 2], dtype=int32)}

