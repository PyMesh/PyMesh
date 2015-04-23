import numpy as np
from numpy.linalg import norm

import PyMeshUtils

class Boundary(object):
    def __init__(self, mesh):
        self.__mesh = mesh;
        if mesh.num_voxels == 0:
            self.__init_surface_boundaries();
        else:
            self.__init_volume_boundaries();

    def __init_surface_boundaries(self):
        self.__add_attribute("vertex_normal");
        surface_boundary  = \
                PyMeshUtils.Boundary.extract_surface_boundary(self.__mesh.raw_mesh);
        boundaries = surface_boundary.get_boundaries();
        assert(boundaries.shape[1] == 2);

        self.boundary_nodes = surface_boundary.get_boundary_nodes().ravel();
        self.boundaries = boundaries;

        v1 = self.__mesh.vertices_ref[boundaries[:,0]];
        v2 = self.__mesh.vertices_ref[boundaries[:,1]];
        self.__boundary_areas = norm(v1 - v2, axis=1);
        self.__boundary_node_areas = np.zeros(self.__mesh.num_vertices);
        for i, bd in enumerate(boundaries):
            self.__boundary_node_areas[bd] += self.__boundary_areas[i];
        self.__boundary_node_areas /= 2.0;
        self.__boundary_centers = (v1 + v2) / 2.0;
        self.__boundary_normals = np.zeros_like(v1);
        self.__boundary_normals[:,0] = v1[:,1] - v2[:,1];
        self.__boundary_normals[:,1] = v2[:,0] - v1[:,0];
        self.__boundary_normals = np.divide(
                self.__boundary_normals, self.__boundary_areas[:,np.newaxis]);

    def __init_volume_boundaries(self):
        self.__add_attribute("face_area");
        self.__add_attribute("vertex_area");
        self.__add_attribute("vertex_normal");
        self.__add_attribute("face_centroid");
        self.__add_attribute("face_normal");

        self.boundaries = self.__mesh.faces_ref;
        self.boundary_nodes = np.unique(self.boundaries.ravel());
        self.__boundary_areas = self.__mesh.get_attribute("face_area").ravel();
        self.__boundary_centers = self.__mesh.get_face_attribute("face_centroid");
        self.__boundary_normals = self.__mesh.get_face_attribute("face_normal");
        self.__boundary_node_areas = self.__mesh.get_attribute("vertex_area").ravel();

    def __add_attribute(self, attr_name):
        if not self.__mesh.has_attribute(attr_name):
            self.__mesh.add_attribute(attr_name);

    @property
    def num_boundary_nodes(self):
        return len(self.boundary_nodes);

    @property
    def num_boundaries(self):
        return len(self.boundaries);

    @property
    def boundary_areas(self):
        return self.__boundary_areas;

    @property
    def boundary_centers(self):
        return self.__boundary_centers;

    @property
    def boundary_normals(self):
        return self.__boundary_normals;

    @property
    def boundary_node_areas(self):
        return self.__boundary_node_areas;

    @property
    def boundary_node_normals(self):
        return self.__mesh.get_attribute("vertex_normal")\
                .reshape((-1, self.__mesh.dim));

