import os.path
import sys
import numpy as np

import PyMesh
import PyMeshUtils

class Mesh(object):
    """ A generic representation of a surface or volume mesh.

    Attributes:

        vertices: A :math:`(N_v \\times D)` array of vertex coordinates, where
            :math:`N_v` is the number of vertices, :math:`D` is the dimention
            of the embedding space (:math:`D` must be either 2 or 3).

        faces: A :math:`(N_f \\times F_d)` array of vertex indices that represents
            a generalized array of faces, where
            :math:`N_f` is the number of faces, :math:`F_d` is the number of
            vertex per face.  Only triangles (:math:`F_d=3`) and quad
            (:math:`F_d=4`) faces are supported.

        voxels: A :math:`(N_V \\times V_d)` array of vertex indices that
            represents an array of generalized voxels, where :math:`N_V` is the
            number of voxels, :math:`V_d` is the number of vertex per voxel.
            Only tetrahedron (:math:`V_d=4`) and hexahedron (:math:`V_d=8`) are
            supported for now.

        num_vertices: Number of vertices (:math:`N_v`).
        num_faces:    Number of faces (:math:`N_f`).
        num_voxels:   Number of voxels (:math:`N_V`).
        dim:          Dimention of the embedding space (:math:`D`).
        vertex_per_face:  Number of vertices in each face (:math:`F_d`).
        vertex_per_voxel: Number of vertices in each voxel (:math:`V_d`).

        attribute_names: Names of all attribute associated with this mesh.
        bbox: A :math:`(2 \\times D)` array where the first row is the
            minimum values of each vertex coordinates, and the second row is the
            maximum values.


        nodes:    Same as :py:attr:`vertices`.
        elements: Array of elements of the mesh.  Same as :py:attr:`faces` for
            surface mesh, or :py:attr:`voxels` for volume mesh.

        num_nodes:         Number of nodes.
        num_elements:      Number of elements.
        nodes_per_element: Number of nodes in each element.
        element_volumes:   An array representing volumes of each element.

        num_components:         Number of vertex-connected_components.
        num_surface_components: Number of edge-connected components.
        num_volume_components:  Number of face-connected components.


        vertices_ref: Same as :py:attr:`vertices` but the return value is a
            reference.
        faces_ref:    Same as :py:attr:`faces` but the return value is a
            reference.
        voxels_ref:   Same as :py:attr:`voxels` but the return value is a
            reference.
        nodes_ref:    Same as :py:attr:`nodes` but the return value is a
            reference.
        elements_ref: Same as :py:attr:`elements` but the return value is a
            reference.


    """

    def __init__(self, raw_mesh):
        """ Private constructor

        Please see :py:mod:`meshio` for methods of loading and saving mesh.
        """
        self.__mesh = raw_mesh;

    def add_attribute(self, name):
        """ Add an attribute to mesh.
        """
        self.__mesh.add_attribute(name);

    def has_attribute(self, name):
        """ Check if an attribute exists.
        """
        return self.__mesh.has_attribute(name);

    def get_attribute(self, name):
        """ Return attribute values in a flattened array.
        """
        return np.copy(self.__mesh.get_attribute(name).ravel());

    def get_attribute_ref(self, name):
        """ Return attribute values in a flattened array.
        """
        return self.__mesh.get_attribute(name).ravel();

    def get_vertex_attribute(self, name):
        """ Same as :py:meth:`.get_attribute` but reshaped to have
        :py:attr:`num_vertices` rows.
        """
        if self.num_vertices == 0:
            return self.__mesh.get_attribute(name);
        else:
            return self.__mesh.get_attribute(name).reshape(
                    (self.num_vertices, -1), order="C");

    def get_face_attribute(self, name):
        """ Same as :py:meth:`.get_attribute` but reshaped to have
        :py:attr:`num_faces` rows.
        """
        if self.num_faces == 0:
            return self.__mesh.get_attribute(name);
        else:
            return self.__mesh.get_attribute(name).reshape(
                    (self.num_faces, -1), order="C");

    def get_voxel_attribute(self, name):
        """ Same as :py:meth:`.get_attribute` but reshaped to have
        :py:attr:`num_voxels` rows.
        """
        if self.num_voxels == 0:
            return self.__mesh.get_attribute(name);
        else:
            return self.__mesh.get_attribute(name).reshape(
                    (self.num_voxels, -1), order="C");

    def set_attribute(self, name, val):
        """ Set attribute to the given value.
        """
        self.__mesh.set_attribute(name, val);

    def get_attribute_names(self):
        """ Get names of all attributes associated with this mesh.
        """
        return self.__mesh.get_attribute_names();

    def enable_connectivity(self):
        self.__mesh.enable_connectivity();

    def get_vertex_adjacent_vertices(self, vi):
        return self.__mesh.get_vertex_adjacent_vertices(vi).ravel();

    def get_vertex_adjacent_faces(self, vi):
        return self.__mesh.get_vertex_adjacent_faces(vi).ravel();

    def get_vertex_adjacent_voxels(self, vi):
        return self.__mesh.get_vertex_adjacent_voxels(vi).ravel();

    def get_face_adjacent_faces(self, fi):
        return self.__mesh.get_face_adjacent_faces(fi).ravel();

    def get_face_adjacent_voxels(self, fi):
        return self.__mesh.get_face_adjacent_voxels(fi).ravel();

    def get_voxel_adjacent_faces(self, Vi):
        return self.__mesh.get_voxel_adjacent_faces(Vi).ravel();

    def get_voxel_adjacent_voxels(self, Vi):
        return self.__mesh.get_voxel_adjacent_voxels(Vi).ravel();

    def is_manifold(self):
        """ Return true iff this mesh is both vertex-manifold and edge-manifold.
        """
        return self.is_vertex_manifold() and self.is_edge_manifold();

    def is_vertex_manifold(self):
        """ Return true iff this mesh is vertex-manifold.

        A mesh is vertex-manifold if the 1-ring neighborhood of each vertex is
        a topological disk.
        """
        return self._extra_info.is_vertex_manifold();

    def is_edge_manifold(self):
        """ Return true iff this mesh is edge-manifold.

        A mesh is edge-manifold if there are exactly 2 incident faces for all
        non-border edges.  Border edges, by definition, only have 1 incident
        face.
        """
        return self._extra_info.is_edge_manifold();

    def is_closed(self):
        """ Return true iff this mesh is closed.

        A closed mesh constains no border.  I.e. all edges have at least 2
        incident faces.
        """
        return self._extra_info.is_closed();

    def is_oriented(self):
        """ Return true iff the mesh is consistently oriented.

        That is all non-bonary edges must represent locally 2-manifold or
        intersection of 2-manifold surfaces.
        """
        return self._extra_info.is_oriented();

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
        if self.num_vertices == 0:
            raise RuntimeError("Cannot compute bbox with 0 vertices!");
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
        try:
            return self.__extra_info;
        except AttributeError:
            self.__extra_info = PyMeshUtils.MeshChecker(
                    self.vertices_ref, self.faces_ref, self.voxels_ref);
            return self.__extra_info;

    @property
    def num_components(self):
        return self._extra_info.get_num_connected_components();

    @property
    def num_surface_components(self):
        return self._extra_info.get_num_connected_surface_components();

    @property
    def num_volume_components(self):
        return self._extra_info.get_num_connected_volume_components();

    @property
    def num_isolated_vertices(self):
        return self._extra_info.get_num_isolated_vertices();

    @property
    def num_duplicated_faces(self):
        return self._extra_info.get_num_duplicated_faces();

    @property
    def num_boundary_edges(self):
        return self._extra_info.get_num_boundary_edges();

    @property
    def boundary_edges(self):
        return self._extra_info.get_boundary_edges();

    @property
    def boundary_vertices(self):
        return np.unique(self.boundary_edges.ravel());

    @property
    def num_boundary_loops(self):
        if self._extra_info.has_complex_boundary():
            return -1;
        else:
            return self._extra_info.get_num_boundary_loops();

    @property
    def boundary_loops(self):
        if self._extra_info.has_complex_boundary():
            return None
        else:
            return self._extra_info.get_boundary_loops();

    @property
    def euler_characteristic(self):
        return self._extra_info.get_euler_characteristic();

    @property
    def genus(self):
        return self._extra_info.get_genus();

    @property
    def volume(self):
        return self._extra_info.compute_signed_volume_from_surface();

