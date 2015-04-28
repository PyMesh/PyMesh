import os.path
import sys
import numpy as np

import PyMesh
import PyMeshUtils

class Mesh(object):
    """
    A very thin wrapper around the mesh structure defined by PyMesh module.
    """
    def __init__(self, raw_mesh):
        self.__mesh = raw_mesh;

    def add_attribute(self, name):
        self.__mesh.add_attribute(name);

    def has_attribute(self, name):
        return self.__mesh.has_attribute(name);

    def get_attribute(self, name):
        return self.__mesh.get_attribute(name);

    def get_vertex_attribute(self, name):
        return self.__mesh.get_attribute(name).reshape(
                (self.num_vertices, -1), order="");

    def get_face_attribute(self, name):
        return self.__mesh.get_attribute(name).reshape(
                (self.num_faces, -1), order="C");

    def get_voxel_attribute(self, name):
        return self.__mesh.get_attribute(name).reshape(
                (self.num_voxels, -1), order="C");

    def set_attribute(self, name, val):
        self.__mesh.set_attribute(name, val);

    def get_attribute_names(self):
        return self.__mesh.get_attribute_names();

    def is_manifold(self):
        return self.is_vertex_manifold() and self.is_edge_manifold();

    def is_vertex_manifold(self):
        return self._extra_info.is_vertex_manifold();

    def is_edge_manifold(self):
        return self._extra_info.is_edge_manifold();

    def is_closed(self):
        return self._extra_info.is_closed();

    @property
    def vertices_ref(self):
        return self.__mesh.get_vertices().reshape(
                (-1,self.dim), order="C");

    @property
    def vertices(self):
        return np.copy(self.vertices_ref);

    @property
    def faces_ref(self):
        return self.__mesh.get_faces().reshape(
                (-1, self.vertex_per_face), order="C");

    @property
    def faces(self):
        return np.copy(self.faces_ref);

    @property
    def voxels_ref(self):
        if self.num_voxels == 0:
            return self.__mesh.get_voxels();
        else:
            return self.__mesh.get_voxels().reshape(
                    (-1, self.vertex_per_voxel), order="C");

    @property
    def voxels(self):
        return np.copy(self.voxels_ref);

    @property
    def num_vertices(self):
        return self.__mesh.get_num_vertices();

    @property
    def num_faces(self):
        return self.__mesh.get_num_faces();

    @property
    def num_voxels(self):
        return self.__mesh.get_num_voxels();

    @property
    def dim(self):
        return self.__mesh.get_dim();

    @property
    def vertex_per_face(self):
        return self.__mesh.get_vertex_per_face();

    @property
    def vertex_per_voxel(self):
        return self.__mesh.get_vertex_per_voxel();

    @property
    def bbox(self):
        vts = self.vertices_ref.reshape((-1,self.dim), order='C');
        bmin = np.amin(vts, axis=0);
        bmax = np.amax(vts, axis=0);
        return bmin, bmax;

    @property
    def raw_mesh(self):
        return self.__mesh;

    @property
    def nodes_ref(self):
        return self.vertices_ref;

    @property
    def nodes(self):
        return self.vertices;

    @property
    def elements_ref(self):
        if self.num_voxels == 0:
            return self.faces_ref;
        else:
            return self.voxels_ref;

    @property
    def elements(self):
        if self.num_voxels == 0:
            return self.faces;
        else:
            return self.voxels;

    @property
    def num_nodes(self):
        return self.num_vertices;

    @property
    def num_elements(self):
        if self.num_voxels == 0:
            return self.num_faces;
        else:
            return self.num_voxels;

    @property
    def nodes_per_element(self):
        if self.num_voxels == 0:
            return self.vertex_per_face;
        else:
            return self.vertex_per_voxel;

    @property
    def element_volumes(self):
        if self.num_voxels == 0:
            return self.get_attribute("face_area");
        else:
            return self.get_attribute("voxel_volume");

    @property
    def attribute_names(self):
        return self.get_attribute_names();

    @property
    def _extra_info(self):
        if not hasattr(self, "__extra_info"):
            self.__extra_info = PyMeshUtils.MeshChecker(
                    self.vertices_ref, self.faces_ref);
        return self.__extra_info;

    @property
    def num_components(self):
        return self._extra_info.get_num_connected_components();
