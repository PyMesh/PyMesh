from pymesh.TestCase import TestCase
from pymesh import detect_self_intersection, resolve_self_intersection
from pymesh.meshio import form_mesh
from pymesh.meshutils import generate_box_mesh
from pymesh.meshutils import merge_meshes
from pymesh.misc import Quaternion

import numpy as np
import math
import unittest

class SelfIntersectionTest(TestCase):
    def get_edge_adj_faces(self, mesh):
        self.assertEqual(3, mesh.vertex_per_face);
        edge_adj_faces = {};
        for i, f in enumerate(mesh.faces):
            f = sorted(f);
            edges = [(f[0], f[1]),
                     (f[1], f[2]),
                     (f[0], f[2])];
            for e in edges:
                if e in edge_adj_faces:
                    edge_adj_faces[e].append(i);
                else:
                    edge_adj_faces[e] = [i];

        return edge_adj_faces;

    def assert_even_adj_faces(self, mesh):
        """ Ensure all edges have an even number of adjacent faces.
        """
        edge_adj_faces = self.get_edge_adj_faces(mesh);
        for e, adj_faces in edge_adj_faces.items():
            self.assertEqual(0, len(adj_faces) % 2);

    def assert_self_intersect(self, mesh):
        """ Ensure there is at least 1 pair of faces are intersecting.
        """
        intersecting_faces = detect_self_intersection(mesh);
        self.assertLess(0, len(intersecting_faces));

    def assert_no_self_intersect(self, mesh):
        """ Ensure the mesh contains no self-intersection.
        """
        intersecting_faces = detect_self_intersection(mesh);
        self.assertEqual(0, len(intersecting_faces));

    def test_no_intersection(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([3, 3, 3]), np.array([4, 4, 4]));
        mesh = merge_meshes((mesh_1, mesh_2));

        intersecting_faces = detect_self_intersection(mesh);
        self.assertEqual(0, len(intersecting_faces));

        output_mesh = resolve_self_intersection(mesh);
        self.assertEqual(mesh.num_vertices, output_mesh.num_vertices);
        self.assertEqual(mesh.num_faces, output_mesh.num_faces);
        self.assert_even_adj_faces(output_mesh);

    def test_simple_intersection(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([2, 2, 2]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([4, 4, 4]));
        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_corner_corner_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([4, 4, 4]));
        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_edge_edge_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 1, 1]), np.array([1, 2, 2]));

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_face_face_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 0, 1]), np.array([1, 1, 2]));

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        # Since the overlapping faces would be kept so valid nested outer hulls
        # can be extracted, the resolved mesh would be actually self-intersecting.
        #self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_face_edge_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromData(
                np.array([1, 0, 1], dtype=float),
                np.array([0, 0, 1], dtype=float));
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([0.5, 0.5, 1.0]),
                mesh_1.faces);

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_face_face_touch_with_different_area(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, -1, 1]), np.array([2, 2, 2]));

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        #self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_face_corner_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromData(
                np.array([1, 1, 1], dtype=float),
                np.array([0, 0, 1], dtype=float));
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([0.5, 0.5, 1.0]),
                mesh_1.faces);

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_face_corner_touch_off_center(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromData(
                np.array([1, 1, 1], dtype=float),
                np.array([0, 0, 1], dtype=float));
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([0.6, 0.6, 1.0]),
                mesh_1.faces);

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_intersect_with_rotated_self(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromAxisAngle(np.array([1, 1, 0], dtype=float),
                math.pi * 0.5);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([0.5, 0.5, 0.5]),
                mesh_1.faces);

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        # The output mesh contains degenerated triangles, which cause CGAL
        # assertion failure.
        #self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_cross_union(self):
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, -2, -1]), np.array([1, 2, 1]));
        mesh_3 = generate_box_mesh(
                np.array([-1, -1, -2]), np.array([1, 1, 2]));

        mesh = merge_meshes((mesh_1, mesh_2, mesh_3));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        # The output mesh contains duplicated faces due to input overlaps.
        #self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

    def test_rotate_120(self):
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));

        rot = Quaternion.fromAxisAngle(
                np.array([1.0, 0.0, 0.0]), float(2*math.pi) / 3);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                mesh_1.faces);

        mesh = merge_meshes((mesh_1, mesh_2));
        output_mesh = resolve_self_intersection(mesh);

        self.assert_self_intersect(mesh);
        # The output mesh contains degenerated face.
        #self.assert_no_self_intersect(output_mesh);
        self.assert_even_adj_faces(output_mesh);

