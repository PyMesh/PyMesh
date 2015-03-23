import numpy as np

from Mesh import Mesh
import PyMesh

def load_mesh(filename):
    factory = PyMesh.MeshFactory();
    factory.load_file(filename);
    factory.drop_zero_dim();
    return Mesh(factory.create());

def form_mesh(vertices, faces, voxels=np.array([])):
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

def save_mesh_raw(filename, vertices, faces, voxels=np.zeros((0,4))):
    assert(vertices is not None);
    assert(faces is not None);
    assert(voxels is not None);

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
    writer.write(
            vertices.ravel(order="C"),
            faces.ravel(order="C"),
            voxels.ravel(order="C"),
            dim,
            num_vertex_per_face,
            num_vertex_per_voxel);

def save_mesh(filename, mesh, *attributes):
    writer = PyMesh.MeshWriter.create_writer(filename);
    for attr in attributes:
        if not mesh.has_attribute(attr):
            raise KeyError("Attribute {} is not found in mesh".format(attr));
        writer.with_attribute(attr);
    writer.write_mesh(mesh.raw_mesh);

def save_mesh_ascii(filename, mesh, *attributes):
    writer = PyMesh.MeshWriter.create_writer(filename);
    for attr in attributes:
        if not mesh.has_attribute(attr):
            raise KeyError("Attribute {} is not found in mesh".format(attr));
        writer.with_attribute(attr);
    writer.in_ascii();
    writer.write_mesh(mesh.raw_mesh);
