import os.path
import sys
import numpy as np

import PyMesh
from Boundary import Boundary

class Mesh(object):
    """
    A very thin wrapper around the mesh structure defined by PyMesh module.
    """
    def __init__(self, raw_mesh):
        self.__mesh = raw_mesh;
        self.__boundary = Boundary(self);

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

    @property
    def vertices(self):
        return self.__mesh.get_vertices().reshape(
                (-1,self.dim), order="C");

    @property
    def faces(self):
        return self.__mesh.get_faces().reshape(
                (-1, self.vertex_per_face), order="C");

    @property
    def voxels(self):
        if self.num_voxels == 0:
            return self.__mesh.get_voxels();
        else:
            return self.__mesh.get_voxels().reshape(
                    (-1, self.vertex_per_voxel), order="C");

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
        vts = self.vertices.reshape((-1,self.dim), order='C');
        bmin = np.amin(vts, axis=0);
        bmax = np.amax(vts, axis=0);
        return bmin, bmax;

    @property
    def raw_mesh(self):
        return self.__mesh;

    @property
    def nodes(self):
        return self.vertices;

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
    def num_boundary_nodes(self):
        return self.__boundary.num_boundary_nodes;

    @property
    def boundary_nodes(self):
        return self.__boundary.boundary_nodes;

    @property
    def num_boundaries(self):
        return self.__boundary.num_boundaries;

    @property
    def boundaries(self):
        return self.__boundary.boundaries;

    @property
    def boundary_areas(self):
        return self.__boundary.boundary_areas;

    @property
    def boundary_centers(self):
        return self.__boundary.boundary_centers;

    @property
    def boundary_normals(self):
        return self.__boundary.boundary_normals;

    @property
    def boundary_node_areas(self):
        return self.__boundary.boundary_node_areas;

    @property
    def boundary_node_normals(self):
        return self.__boundary.boundary_node_normals;


