from pymesh import generate_icosphere
from pymesh.meshutils import collapse_short_edges_raw, collapse_short_edges
from pymesh.TestCase import TestCase

import numpy as np
import numpy.testing
import unittest

class CollapseShortEdgesTest(TestCase):
    def test_simple_3D(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, info = collapse_short_edges_raw(
                vertices, faces, 0.1);

        numpy.testing.assert_array_equal(vertices, out_vertices);
        numpy.testing.assert_array_equal(faces, out_faces);
        self.assertEqual(0, info["num_edge_collapsed"]);
        self.assert_array_equal([0], info["source_face_index"]);

    def test_collapse_all_3D(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, info = collapse_short_edges_raw(
                vertices, faces, 1.1);

        self.assertEqual(0, len(out_vertices));
        self.assertEqual(0, len(out_faces));
        self.assertEqual(1, info["num_edge_collapsed"]);
        self.assert_array_equal([], info["source_face_index"]);

    def test_slim_triangles(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            [-0.1,-0.1,-0.1],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [0, 3, 1],
            [0, 2, 3],
            ], dtype=int);

        out_vertices, out_faces, info = collapse_short_edges_raw(
                vertices, faces, 0.5);

        self.assertEqual(3, len(out_vertices));
        self.assertEqual(1, len(out_faces));
        self.assertEqual(1, info["num_edge_collapsed"]);
        self.assert_array_equal([0], info["source_face_index"]);

    def test_degenerated_triangles(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            [-0.1,-0.1,-0.1],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [0, 3, 1],
            [0, 2, 3],
            [0, 0, 1],
            [2, 3, 3]
            ], dtype=int);

        out_vertices, out_faces, info = collapse_short_edges_raw(
                vertices, faces, 0.5);

        self.assertEqual(3, len(out_vertices));
        self.assertEqual(1, len(out_faces));
        self.assertEqual(1, info["num_edge_collapsed"]);
        self.assert_array_equal([0], info["source_face_index"]);

    def test_simple_2D(self):
        vertices = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.0, 1.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, info = collapse_short_edges_raw(
                vertices, faces, 0.1);

        numpy.testing.assert_array_equal(vertices, out_vertices);
        numpy.testing.assert_array_equal(faces, out_faces);
        self.assertEqual(0, info["num_edge_collapsed"]);
        self.assert_array_equal([0], info["source_face_index"]);

    def test_tiny_mesh(self):
        """ Edge collapse performed on a tiny icosphere.
        """
        mesh = generate_icosphere(1e-6, [0.0, 0.0, 0.0]);
        mesh, info = collapse_short_edges(mesh, 0.1);

        self.assertTrue("face_sources" in mesh.attribute_names);
        self.assertEqual(0, mesh.num_vertices);
        self.assertEqual(0, mesh.num_faces);
        self.assertEqual(0, mesh.num_voxels);

