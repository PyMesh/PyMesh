import numpy as np

from Mesh import Mesh
import PyMesh

def load_mesh(filename):
    """ Load mesh from a file.

    Args:
        filename: input filename.  File format is auto detected based on
            extension.

    Returns:
        A Mesh object.
    """
    factory = PyMesh.MeshFactory();
    factory.load_file(filename);
    factory.drop_zero_dim();
    return Mesh(factory.create());

def form_mesh(vertices, faces, voxels=None):
    """ Convert raw mesh data into a Mesh object.

    Args:
        vertices: ndarray of floats with size (num_vertices, dim).
        faces: ndarray of ints with size (num_faces, vertex_per_face).
        voxels: optional ndarray of ints with size (num_voxels,
            vertex_per_voxel).  Use `None` for forming surface meshes.

    Returns:
        A Mesh object.
    """
    if voxels is None:
        voxels = np.zeros((0, 4));
    dim = vertices.shape[1];
    factory = PyMesh.MeshFactory();
    if dim == 3:
        factory.load_data(
                vertices.ravel(order="C"),
                faces.ravel(order="C"),
                voxels.ravel(order="C"), 3, 3, 4);
    elif dim == 2:
        factory.load_data(
                vertices.ravel(order="C"),
                faces.ravel(order="C"),
                voxels.ravel(order="C"), 2, 3, 4);
    return Mesh(factory.create());

def save_mesh_raw(filename, vertices, faces, voxels=None, **setting):
    """ Save raw mesh to file.

    Args:
        filename (:py:class:`str`): Output file.  File format is auto detected from extension.
        vertices (:py:class:`numpy.ndarray`): Array of floats with size (num_vertices, dim).
        faces (:py:class:`numpy.ndarray`): Arrayof ints with size (num_faces, vertex_per_face).
        voxels (:py:class:`numpy.ndarray`): (optional) ndarray of ints with size (num_voxels,
            vertex_per_voxel).  Use `None` for forming surface meshes.
        **setting (:py:class:`dict`): (optional) The following keys are recognized.

            * ascii: whether to use ascii encoding, default is false.
            * use_float: store scalars as float instead of double, default is
              false.
    """
    if voxels is None:
        voxels = np.zeros((0, 4));

    if not isinstance(vertices, np.ndarray):
        vertices = np.array(vertices, copy=False, order='C');
    if not isinstance(faces, np.ndarray):
        faces = np.array(faces, copy=False, order='C');
    if not isinstance(voxels, np.ndarray):
        voxels = np.array(voxels, copy=False, order='C');

    dim = vertices.shape[1];
    num_vertex_per_face = faces.shape[1];
    num_vertex_per_voxel = voxels.shape[1];

    writer = PyMesh.MeshWriter.create_writer(filename);
    if setting.get("ascii", False):
        writer.in_ascii();
    if setting.get("use_float", False):
        writer.use_float();
    writer.write(
            vertices.ravel(order="C"),
            faces.ravel(order="C"),
            voxels.ravel(order="C"),
            dim,
            num_vertex_per_face,
            num_vertex_per_voxel);

def save_mesh(filename, mesh, *attributes, **setting):
    """ Save mesh to file.

    Args:
        filename (:py:class:`str`): Output file.  File format is auto detected from extension.
        mesh (:py:class:`Mesh`): Mesh object.
        *attributes (:py:class:`list`): (optional) Attribute names to be saved.
            This field would be ignored if the output format does not support
            attributes (e.g. **.obj** and **.stl** files)
        **setting (:py:class:`dict`): (optional) The following keys are recognized.

            * ascii: whether to use ascii encoding, default is false.
            * use_float: store scalars as float instead of double, default is
              false.

    Raises:
        KeyError: Attributes cannot be found in mesh.
    """
    writer = PyMesh.MeshWriter.create_writer(filename);
    for attr in attributes:
        if not mesh.has_attribute(attr):
            raise KeyError("Attribute {} is not found in mesh".format(attr));
        writer.with_attribute(attr);
    if setting.get("ascii", False):
        writer.in_ascii();
    if setting.get("use_float", False):
        writer.use_float();
    writer.write_mesh(mesh.raw_mesh);
