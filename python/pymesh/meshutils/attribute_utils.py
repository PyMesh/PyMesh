import PyMeshUtils
import numpy as np

def convert_to_vertex_attribute(mesh, attr):
    return PyMeshUtils.convert_to_vertex_attribute(mesh.raw_mesh, attr);

def convert_to_vertex_attribute_from_name(mesh, name):
    return PyMeshUtils.convert_to_vertex_attribute_from_name(mesh.raw_mesh, name);

def convert_to_face_attribute(mesh, attr):
    return PyMeshUtils.convert_to_face_attribute(mesh.raw_mesh, attr);

def convert_to_face_attribute_from_name(mesh, name):
    return PyMeshUtils.convert_to_face_attribute_from_name(mesh.raw_mesh, name);

def convert_to_voxel_attribute(mesh, attr):
    return PyMeshUtils.convert_to_voxel_attribute(mesh.raw_mesh, attr);

def convert_to_voxel_attribute_from_name(mesh, name):
    return PyMeshUtils.convert_to_voxel_attribute_from_name(mesh.raw_mesh, name);
