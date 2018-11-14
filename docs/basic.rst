Basic Usage
===========

PyMesh is rapid prototyping library focused on processing and generating 3D
meshes.  The `Mesh` class is the core data structure and is used by all
modules.

Mesh Data Structure
-------------------

In PyMesh, a `Mesh` consists of 3 parts: geometry, connectivity and
attributes.

* Geometry consists of vertices, faces and generalized voxels (i.e. a volume
  element such as tetrahedron or hexahedron).  The dimension of the embedding
  space, face type, voxel type can all be inferred from the geometry data.  It
  is possible for a mesh to consist of 0 vertices or 0 faces or 0 voxels.

* The connectivity contains adjacency information, including vertex-vertex,
  vertex-face, vertex-voxel, face-face, face-voxel and voxel-voxel adjacencies.

* Attributes are arbitrary value field assigned to a mesh.  One could assign a scalar
  or vector for each vertex/face/voxel.  There are a number predefined attributes.


Loading Mesh
------------

From file::

    >>> mesh = pymesh.load_mesh("model.obj")

PyMesh supports parsing the following formats: .obj, .ply, .off, .stl, .mesh,
.node, .poly and .msh.

From raw data::

    >>> # for surface mesh:
    >>> mesh = pymesh.form_mesh(vertices, faces)

    >>> # for volume mesh:
    >>> mesh = pymesh.form_mesh(vertices, faces, voxels)

where ``vertices``, ``faces`` and ``voxels`` are of type
:py:class:`numpy.ndarray`.  One vertex/face/voxel per row.

Accessing Mesh Data
-------------------

Geometry data can be directly accessed::

    >>> print(mesh.num_vertices, mesh.num_faces, mesh.num_voxels)
    (8, 12, 6)

    >>> print(mesh.dim, mesh.vertex_per_face, mesh.vertex_per_voxel)
    (3, 3, 4)

    >>> mesh.vertices
    array([[-1., -1.,  1.],
           ...
           [ 1.,  1.,  1.]])

    >>> mesh.faces
    array([[0, 1, 2],
           ...
           [4, 5, 6]])

    >>> mesh.voxels
    array([[0, 1, 2, 3],
           ...
           [4, 5, 6, 7]])

Connectivity data is disabled by default because it is often not needed.  To
enable it::

    >>> mesh.enable_connectivity();

The connectivity information can be queried using the following methods::

    >>> mesh.get_vertex_adjacent_vertices(vi);
    >>> mesh.get_vertex_adjacent_faces(vi);
    >>> mesh.get_vertex_adjacent_voxels(vi);

    >>> mesh.get_face_adjacent_faces(fi);
    >>> mesh.get_face_adjacent_voxels(fi);

    >>> mesh.get_voxel_adjacent_faces(Vi);
    >>> mesh.get_voxel_adjacent_voxels(Vi);

Using Attributes
----------------

Attributes allow one to attach a scalar or vector fields to the mesh.  For
example, vertex normal could be stored as a mesh attribute where a normal vector
is associated with each vertex.  In addition to vertices, attribute could be
associated with face and voxels.  To create an attribute::

    >>> mesh.add_attribute("attribute_name");

This creates an empty attribute (of length 0) called ``attribute_name``.  To
assign value to the attribute::

    >>> val = np.ones(mesh.num_vertices);
    >>> mesh.set_attribute("attribute_name", val);

Notice that the ``val`` variable is a native python
:py:class:`numpy.ndarray`.  The length of the attribute is used to determine
whether it is a scalar field or vector field.  The length is also used to
determine whether the attribute is assigned to vertices, faces or voxels.

To access a defined attribute::

    >>> attr_val = mesh.get_attribute("attribute_name");
    >>> attr_val
    array([ 1.0,  1.0,  1.0, ...,  1.0, 1.0,  1.0])

The following vertex attributes are predefined:

* ``vertex_normal``: A vector field representing surface normals.  Zero vectors
  are assigned to vertices in the interior.
* ``vertex_volume``: A scalar field representing the lumped volume of each
  vertex (e.g. 1/4 of the total volume of all neighboring tets for tetrahedron
  mesh.).
* ``vertex_area``: A scalar field representing the lumped surface area of each
  vertex (e.g. 1/3 of the total face area of its 1-ring neighborhood).
* ``vertex_laplacian``: A vector field representing the discretized Laplacian
  vector.
* ``vertex_mean_curvature``: A scalar field representing the mean curvature
  field of the mesh.
* ``vertex_gaussian_curvature``: A scalar field representing the Gaussian
  curvature field of the mesh.
* ``vertex_index``: A scalar field representing the index of each vertex.
* ``vertex_valance``: A scalar field representing the valance of each vertex.
* ``vertex_dihedral_angle``: A scalar field representing the max dihedral angle
  of all edges adjacent to this vertex.

The following face attributes are predefined:

* ``face_area``: A scalar field representing face areas.
* ``face_centroid``: A vector field representing the face centroids (i.e.
  average of all corners).
* ``face_circumcenter``: A vector field representing the face circumcenters
  (defined for triangle faces only).
* ``face_index``: A scalar field representing the index of each face.
* ``face_normal``: A vector field representing the normal vector of each face.
* ``face_voronoi_area``: A vector field representing the Voronoi area of each
  corner of the face.

The following voxel attributes are predefined:

* ``voxel_index``: A scalar field representing the index of each voxel.
* ``voxel_volume``: A scalar field representing the volume of each voxel.
* ``voxel_centroid``: A scalar field representing the centroid of each voxel
  (i.e. average of all corners of a voxel).

Predefined attribute does not need to be set::

    >>> mesh.add_attribute("vertex_area")
    >>> mesh.get_attribute("vertex_area")
    array([ 0.56089278,  0.5608997 ,  0.57080866, ...,  5.62381961,
            2.12105028,  0.37581711])

Notice that attribute values are always stored as a 1D array.  For attributes
that represent vector/tensor fields, the attribute values are the flattened
version of the vector field::

    >>> mesh.add_attribute("vertex_normal")
    >>> mesh.get_attribute("vertex_normal")
    array([ 0.35735435, -0.49611438, -0.79130802, ..., -0.79797784,
            0.55299134, -0.23964964])

If an attribute is known to be a per-vertex attribute, one can::

    >>> mesh.get_vertex_attribute("vertex_normal")
    array([[ 0.35735435, -0.49611438, -0.79130802],
           [ 0.41926554, -0.90767626, -0.01844495],
           [-0.64142577,  0.76638469, -0.03503568],
           ..., 
           [-0.64897662, -0.64536558, -0.40290522],
           [-0.92207726, -0.10573231, -0.37228242],
           [-0.79797784,  0.55299134, -0.23964964]])

where attribute values are returned as a 2D matrix.  Each row represents the
value per vertex.

Similarly, per-face and per-voxel attribute can be retrieved using
``get_face_attribute()`` and ``get_voxel_attribute()`` methods.

To retrieve the names of all defined attributes for a given mesh::

    >>> mesh.get_attribute_names()
    ("attribute_name", "vertex_area", "vertex_normal")

Saving Mesh
-----------
The following formats are supported for saving meshes: .obj, .off, .ply, .mesh,
.node, .poly, .stl and .msh.
However, saving in .stl format is strongly discouraged because
`STL files use more disk space and stores less information
<https://medium.com/3d-printing-stories/why-stl-format-is-bad-fea9ecf5e45>`_.
To save a mesh::

    >>> pymesh.save_mesh("filename.obj", mesh);

For certain formats (e.g. .ply, .msh, .stl), it is possible to save either as
an ASCII file or a binary file.  By default, PyMesh will always use the binary
format. To save in ASCII, just set the ``ascii`` argument::

    >>> pymesh.save_mesh("filename.obj", mesh, ascii=True)

In addition, vertex position can be saved using ``double`` or ``float``.  By
default, PyMesh saves in ``double``, to save using ``float``::

    >>> pymesh.save_mesh("filename.obj", mesh, use_float=True)

Mesh attributes can also be saved in .msh and .ply formats.  To save with
attributes::

    >>> pymesh.save_mesh("filename.msh", mesh, attribute_name_1, attribute_name_2, ...)

To save with all defined attributes::

    >>> pymesh.save_mesh("filename.msh", mesh, *mesh.get_attribute_names())

It is also possible to save from raw vertices, faces and voxels::

    >>> # For surface mesh
    >>> pymesh.save_mesh_raw("filename.ply", vertices, faces)

    >>> # For volume mesh
    >>> pymesh.save_mesh_raw("filename.ply", vertices, faces, voxels)

    >>> # In ascii and using float
    >>> pymesh.save_mesh_raw("filename.ply", vertices, faces, voxels,\
            ascii=True, use_float=True)

