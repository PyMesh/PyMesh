from pymesh.TestCase import TestCase
from pymesh import boolean
from pymesh.meshutils import generate_box_mesh
from pymesh.misc import Quaternion
from pymesh.meshio import form_mesh

import numpy as np
import math
import os.path
import unittest

#@unittest.skip("skipped for debugging purposes")
class BooleanTest(TestCase):
    def test_separated_shapes(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([2, 2, 2]), np.array([3, 3, 3]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(0, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "union", "igl");
        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(2, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "difference", "igl");
        self.assert_array_equal(mesh_1.bbox, mesh.bbox);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "symmetric_difference", "igl");
        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(2, mesh.num_components);

    def test_corner_corner_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([3, 3, 3]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(0, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "union", "igl");
        self.assertEqual(15, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "difference", "igl");
        self.assert_array_equal(mesh_1.bbox, mesh.bbox);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "symmetric_difference", "igl");
        self.assertEqual(15, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_edge_edge_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 1, 1]), np.array([1, 2, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(0, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "union", "igl");
        self.assertEqual(14, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "difference", "igl");
        self.assert_array_equal(mesh_1.bbox, mesh.bbox);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "symmetric_difference", "igl");
        self.assertEqual(14, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    #@unittest.skip("Coplanar faces are not handled correctly yet.")
    def test_face_face_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 0, 1]), np.array([1, 1, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(0, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "union", "igl");
        self.assertEqual(12, mesh.num_vertices);
        self.assertEqual(20, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "difference", "igl");
        self.assert_array_equal(mesh_1.bbox, mesh.bbox);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "symmetric_difference", "igl");
        self.assertEqual(12, mesh.num_vertices);
        self.assertEqual(20, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_simple_intersection(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([2, 2, 2]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([3, 3, 3]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "union", "igl");
        self.assertEqual(20, mesh.num_vertices);
        self.assertEqual(36, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "difference", "igl");
        self.assertEqual(14, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

        mesh = boolean(
                mesh_1, mesh_2,
                "symmetric_difference", "igl");
        self.assertEqual(22, mesh.num_vertices);
        self.assertEqual(48, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_eps_corner_intersection(self):
        eps = np.finfo(np.float64).eps;
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]) - eps, np.array([3, 3, 3]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_eps_edge_intersection(self):
        eps = np.finfo(np.float64).eps;
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 1-eps, 1-eps]), np.array([1, 2, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        bbox = mesh.bbox;
        self.assert_array_equal(np.array([0, 1-eps, 1-eps]), bbox[0]);
        self.assert_array_equal(np.array([1, 1, 1]), bbox[1]);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_eps_face_intersection(self):
        eps = np.finfo(np.float64).eps;
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 0, 1-eps]), np.array([1, 1, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");
        bbox = mesh.bbox;
        self.assert_array_equal(np.array([0, 0, 1-eps]), bbox[0]);
        self.assert_array_equal(np.array([1, 1, 1]), bbox[1]);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    def test_edge_edge_touch_with_different_length(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, 1, 1]), np.array([2, 2, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");

        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(0, mesh.num_components);

    #@unittest.skip("Duplicated faces are not handled correctly")
    def test_face_face_touch_with_different_area(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, -1, 1]), np.array([2, 2, 2]));

        mesh = boolean(
                mesh_1, mesh_2,
                "intersection", "igl");

        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(0, mesh.num_components);

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

        mesh = boolean(mesh_1, mesh_2, "union", "igl");

        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(26, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

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

        mesh = boolean(mesh_1, mesh_2, "union", "igl");

        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(26, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

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

        mesh = boolean(mesh_1, mesh_2, "union", "igl");

        self.assertEqual(17, mesh.num_vertices);
        self.assertEqual(30, mesh.num_faces);
        self.assertFalse(mesh.is_manifold());
        self.assertTrue(mesh.is_closed());
        self.assertEqual(1, mesh.num_components);

    #@unittest.skip("this causes crash")
    def test_union_with_rotated_self(self):
        #TODO: bug
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromAxisAngle(np.array([1, 1, 0], dtype=float),
                math.pi * 0.5);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([0.5, 0.5, 0.5]),
                mesh_1.faces);

        mesh = boolean(mesh_1, mesh_2, "union", "igl");

        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

    def test_edge_edge_orthogonal_touch(self):
        eps = np.finfo(float).eps;
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromAxisAngle(np.array([1, 1, 0], dtype=float),
                math.pi * 0.5);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T +
                np.array([-1, -1, 0.5], dtype=float)+
                np.array([-math.sqrt(2)/4.0 + eps,
                    math.sqrt(2)/4.0+eps, 0.0], dtype=float),
                mesh_1.faces);

        mesh = boolean(mesh_1, mesh_2, "union", "igl");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

    def test_intersection_with_self(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh = boolean(mesh_1, mesh_1, "intersection", "igl");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(1, mesh.num_components);

    def test_intersection_with_slighly_rotated_self(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromAxisAngle(np.array([1.0, 1.0, 1.0]), 0.0001);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                mesh_1.faces);

        mesh = boolean(mesh_1, mesh_2, "intersection", "igl");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(1, mesh.num_components);

    def test_union_with_45_rotated_self(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));

        rot = Quaternion.fromAxisAngle(np.array([1.0, 1.0, 1.0]), math.pi/4.0);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                mesh_1.faces);

        mesh = boolean(mesh_1, mesh_2, "union", "igl");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertTrue(1, mesh.num_components);

    def test_nested_mesh(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, -1, -1]), np.array([2, 2, 2]));

        mesh = boolean(mesh_1, mesh_2, "intersection", "igl");
        self.assert_array_equal(mesh_1.bbox, mesh.bbox);

        mesh = boolean(mesh_1, mesh_2, "union", "igl");
        self.assert_array_equal(mesh_2.bbox, mesh.bbox);

        mesh = boolean(mesh_1, mesh_2, "difference", "igl");
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);

        mesh = boolean(mesh_2, mesh_1, "difference", "igl");
        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assert_array_equal(mesh_2.bbox, mesh.bbox);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(2, mesh.num_components);

        mesh = boolean(mesh_1, mesh_2, "symmetric_difference", "igl");
        self.assertEqual(16, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assert_array_equal(mesh_2.bbox, mesh.bbox);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(2, mesh.num_components);

    def test_cross_union(self):
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([-1, -2, -1]), np.array([1, 2, 1]));
        mesh_3 = generate_box_mesh(
                np.array([-1, -1, -2]), np.array([1, 1, 2]));

        mesh = boolean(mesh_1, mesh_2, "union", "igl");
        mesh = boolean(mesh, mesh_3, "union", "igl");

        self.assert_array_equal(
                np.array([[-2, -2, -2], [2, 2, 2]], dtype=float),
                mesh.bbox);
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

    #@unittest.skip("this causes crash")
    def test_rotate_union_120_degrees(self):
        # TODO: Bug
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));

        rot = Quaternion.fromAxisAngle(
                np.array([1.0, 0.0, 0.0]), float(2*math.pi) / 3);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                mesh_1.faces);
        mesh = boolean(mesh_1, mesh_2, "union", "igl");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

    #@unittest.skip("this causes unit test failure due to numerical problem")
    def test_rotation_union_stress(self):
        # TODO: Bug
        N = 2;
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));

        mesh = mesh_1;
        for i in range(N):
            rot = Quaternion.fromAxisAngle(
                    np.array([1.0, 0.0, 0.0]), float(i*2*math.pi) / N);
            mesh_2 = form_mesh(
                    np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                    mesh_1.faces);

            mesh = boolean(mesh, mesh_2, "union", "igl");
            self.assertTrue(mesh.is_closed());
            self.assertTrue(mesh.is_manifold());
            self.assertEqual(1, mesh.num_components);

    #@unittest.skip("this causes crash due to intermediate degenerated triangle")
    def test_rotation_union_stress_2(self):
        # TODO: Bug
        N = 3;
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));

        mesh = mesh_1;
        for i in range(N):
            rot = Quaternion.fromAxisAngle(
                    np.array([1.0, 1.0, 0.0]), float(i*2*math.pi) / N);
            mesh_2 = form_mesh(
                    np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                    mesh_1.faces);
            mesh = boolean(mesh, mesh_2, "union", "igl");

            self.assertTrue(mesh.is_closed());
            self.assertTrue(mesh.is_manifold());
            self.assertEqual(1, mesh.num_components);

    def test_rotation_180(self):
        mesh_1 = generate_box_mesh(
                np.array([-2, -1, -1]), np.array([2, 1, 1]));

        rot = Quaternion.fromAxisAngle(
                np.array([1.0, 1.0, 0.0]), math.pi / 2.0);
        mesh_2 = form_mesh(
                np.dot(rot.to_matrix(), mesh_1.vertices.T).T,
                mesh_1.faces);
        mesh = boolean(mesh_1, mesh_2, "union", "cgal");
        self.assertTrue(mesh.is_closed());
        self.assertTrue(mesh.is_manifold());
        self.assertEqual(1, mesh.num_components);

