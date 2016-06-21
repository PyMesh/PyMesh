Mesh Boolean
============

Boolean operation is one of the fundamental operations for 3D modeling.  It
combines two or more solid shapes (say :math:`A` and :math:`B`)
by checking if a point :math:`x` lies inside of each solid.  Four commonly used
boolean operations are:

* Union: :math:`A \cup B := \{ x \in \mathbb{R}^3 \mid x \in A \textrm{ and } x \in B \}`

* Intersection: :math:`A \cap B := \{ x \in \mathbb{R}^3 \mid x \in A \textrm{ or } x \in B \}`

* Difference: :math:`A \setminus B := \{ x \in \mathbb{R}^3 \mid x \in A \textrm{ and } x \notin B \}`

* Symmetric difference: :math:`A \textrm{ XOR }  B := (A \setminus B) \cup (B \setminus A)`

Interface:
----------

PyMesh provides support for all four operations through third party boolean
*engines*.  For example, computing the union of ``mesh_A`` and ``mesh_B`` can be
achieved with the following snippet::

    >>> output_mesh = pymesh.boolean(mesh_A, mesh_B,
    ...                              operation="union",
    ...                              engine="igl")

The interface is very minimal and self-explanatory.
The available operations are ``"union"``, ``"intersection"``, ``"difference"``
and ``"symmetric_difference"``.  PyMesh supports the following boolean engines:

* ``"igl"``: Boolean module from libigl, which is also the default engine for 3D
  inputs.
* ``"cgal"``: Naf polyhedron implementation from CGAL.
* ``"cork"``: Cork boolean library.
* ``"carve"``: Carve boolean library.
* ``"corefinement"``: The unpublished boolean engine also from CGAL.
* ``"clipper"``: 2D boolean engine for polygons, the default engine for 2D
  inputs.

The following attributes are defined in the ``output_mesh``:

* ``source``: A per-face scalar attribute indicating which input mesh an output
  face belongs to.
* ``source_face``: A per-face scalar attribute representing the combined input
  face index of an output face, where combined input faces are simply the
  concatenation of faces from ``mesh_A`` and ``mesh_B``.

Examples:
---------

Coming soon.

