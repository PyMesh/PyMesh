import pymesh
from pymesh.TestCase import TestCase
from pymesh import triangulate, retriangulate
import numpy as np

class TriangulateTest(TestCase):
    def test_2D(self):
        vertices = np.array([
            [0, 0],
            [1, 0],
            [1, 1],
            [0, 1] ], dtype=float);
        segments = np.array([
            [0, 1],
            [1, 2],
            [2, 3],
            [3, 0] ], dtype=int);

        vertices, faces = triangulate(vertices, segments, 0.1);
        self.assertLess(4, len(vertices));
        self.assertLess(2, len(faces));
        self.assert_array_almost_equal([1.0, 1.0], np.amax(vertices, axis=0));
        self.assert_array_almost_equal([0.0, 0.0], np.amin(vertices, axis=0));

    def test_3D(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 1],
            [1, 1, 1],
            [0, 1, 0] ], dtype=float);
        segments = np.array([
            [0, 1],
            [1, 2],
            [2, 3],
            [3, 0] ], dtype=int);
        vertices, faces = triangulate(vertices, segments, 0.1);
        self.assertLess(4, len(vertices));
        self.assertLess(2, len(faces));
        self.assert_array_almost_equal(
                [1.0, 1.0, 1.0], np.amax(vertices, axis=0));
        self.assert_array_almost_equal(
                [0.0, 0.0, 0.0], np.amin(vertices, axis=0));

    def test_refine(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 1],
            [1, 1, 1],
            [0, 1, 0] ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [0, 2, 3] ]);
        in_mesh = pymesh.form_mesh(vertices, faces);
        out_mesh = retriangulate(in_mesh, 0.1);
        self.assertLess(in_mesh.num_vertices, out_mesh.num_vertices);
        self.assertLess(in_mesh.num_faces, out_mesh.num_faces);
        in_mesh.add_attribute("face_area");
        in_areas = in_mesh.get_attribute("face_area");
        out_mesh.add_attribute("face_area");
        out_areas = out_mesh.get_attribute("face_area");
        self.assertAlmostEqual(np.sum(in_areas), np.sum(out_areas));

