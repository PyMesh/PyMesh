import pymesh
from pymesh.TestCase import TestCase

import numpy as np
import numpy.random

class TestCompression(TestCase):
    def test_simple(self):
        mesh = pymesh.generate_icosphere(1.0, np.zeros(3), 2);
        data = pymesh.compress(mesh);
        mesh2 = pymesh.decompress(data);

        self.assertEqual(mesh.num_vertices, mesh2.num_vertices);
        self.assertEqual(mesh.num_faces, mesh2.num_faces);
        self.assert_array_almost_equal(mesh.bbox, mesh2.bbox);

    def test_point_cloud(self):
        vertices = numpy.random.rand(100, 3);
        mesh = pymesh.form_mesh(vertices, np.zeros((0, 3)));
        data = pymesh.compress(mesh);
        mesh2 = pymesh.decompress(data);

        self.assertEqual(mesh.num_vertices, mesh2.num_vertices);
        self.assertEqual(mesh.num_faces, mesh2.num_faces);
        self.assert_array_almost_equal(mesh.bbox, mesh2.bbox);

    def test_2D(self):
        vertices = numpy.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [1.0, 1.0],
            [0.0, 1.0],
            ]);
        faces = np.array([
            [0, 1, 2],
            [0, 2, 3]
            ]);
        mesh = pymesh.form_mesh(vertices, faces);
        data = pymesh.compress(mesh);
        mesh2 = pymesh.decompress(data);

        self.assertEqual(mesh.num_vertices, mesh2.num_vertices);
        self.assertEqual(mesh.num_faces, mesh2.num_faces);
        self.assert_array_almost_equal(mesh.bbox, mesh2.bbox);

    def test_quad(self):
        vertices = numpy.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [1.0, 1.0],
            [0.0, 1.0],
            ]);
        faces = np.array([
            [0, 1, 2, 3],
            ]);
        mesh = pymesh.form_mesh(vertices, faces);

        with self.assertRaises(RuntimeError):
            data = pymesh.compress(mesh);

    def test_empty_mesh(self):
        vertices = numpy.zeros((0, 3), dtype=float);
        faces = np.zeros((0, 3), dtype=int);
        mesh = pymesh.form_mesh(vertices, faces);

        data = pymesh.compress(mesh);
        mesh2 = pymesh.decompress(data);

        self.assertEqual(mesh.num_vertices, mesh2.num_vertices);
        self.assertEqual(mesh.num_faces, mesh2.num_faces);

    def test_attributes(self):
        mesh = pymesh.generate_icosphere(1.0, np.zeros(3), 2);
        mesh.add_attribute("vertex_normal");
        mesh.add_attribute("face_normal");
        data = pymesh.compress(mesh);
        mesh2 = pymesh.decompress(data);

        self.assertEqual(mesh.num_vertices, mesh2.num_vertices);
        self.assertEqual(mesh.num_faces, mesh2.num_faces);
        self.assert_array_almost_equal(mesh.bbox, mesh2.bbox);
        self.assertTrue(mesh2.has_attribute("vertex_normal"));
        self.assertTrue(mesh2.has_attribute("face_normal"));
