import PyMesh
import numpy as np

def convert_to_vertex_attribute(mesh, attr):
    """ Convert attribute ``attr`` from either per-face or per-voxel attribute
    into per-vertex attribute.

    Args:
        mesh (:py:class:`Mesh`): Input mesh.
        attr (``numpy.ndarray``): #vertices by k matrix of floats.

    Returns:
        Per-vertex attribute. The value at a vertex will be the average of
        the values at its neighboring faces or voxels.
    """
    return PyMesh.convert_to_vertex_attribute(mesh.raw_mesh, attr)

def convert_to_vertex_attribute_from_name(mesh, name):
    """ Same as :py:func:`convert_to_vertex_attribute` except looking up
    attribute values from the input ``mesh`` using ``name``.
    """
    return PyMesh.convert_to_vertex_attribute_from_name(mesh.raw_mesh, name)

def convert_to_face_attribute(mesh, attr):
    """ Convert attribute ``attr`` from either per-vertex or per-voxel attribute
    into per-face attribute.

    Args:
        mesh (:py:class:`Mesh`): Input mesh.
        attr (``numpy.ndarray``): #faces by k matrix of floats.

    Returns:
        Per-face attribute. The value at a face will be the average of
        the values at its neighboring vertices or its neighboring voxels.
    """
    return PyMesh.convert_to_face_attribute(mesh.raw_mesh, attr)

def convert_to_face_attribute_from_name(mesh, name):
    """ Same as :py:func:`convert_to_face_attribute` except looking up
    attribute values from the input ``mesh`` using ``name``.
    """
    return PyMesh.convert_to_face_attribute_from_name(mesh.raw_mesh, name)

def convert_to_voxel_attribute(mesh, attr):
    """ Convert attribute ``attr`` from either per-vertex or per-face attribute
    into per-voxel attribute.

    Args:
        mesh (:py:class:`Mesh`): Input mesh.
        attr (``numpy.ndarray``): #voxel by k matrix of floats.

    Returns:
        Per-voxel attribute. The value at a voxel will be the average of
        the values at its neighboring vertices or faces.
    """
    return PyMesh.convert_to_voxel_attribute(mesh.raw_mesh, attr)

def convert_to_voxel_attribute_from_name(mesh, name):
    """ Same as :py:func:`convert_to_voxel_attribute` except looking up
    attribute values from the input ``mesh`` using ``name``.
    """
    return PyMesh.convert_to_voxel_attribute_from_name(mesh.raw_mesh, name)
