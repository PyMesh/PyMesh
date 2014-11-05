import numpy as np
from numpy.linalg import det
import os
import os.path
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

import PyMesh
import PyConvexHull

class ConvexHullEngineTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        return factory.create_shared();

    def assertBBoxEqual(self, pts1, pts2):
        bbox_min_1 = np.amin(pts1, axis=0);
        bbox_max_1 = np.amax(pts1, axis=0);
        bbox_min_2 = np.amin(pts2, axis=0);
        bbox_max_2 = np.amax(pts2, axis=0);
        self.assertListEqual(bbox_min_1.tolist(), bbox_min_2.tolist());
        self.assertListEqual(bbox_max_1.tolist(), bbox_max_2.tolist());

    def assertPositiveVolume(self, vertices, faces):
        center = 0.5 * (np.amin(vertices, axis=0) + np.amax(vertices, axis=0));
        for face in faces:
            corners = vertices[face] - center;
            volume = det(corners);
            self.assertGreater(volume, 0.0);

    def test_cgal_cube(self):
        if not PyConvexHull.ConvexHullEngine.supports("cgal"):
            return;

        mesh = self.load_mesh("cube.msh");
        points = mesh.get_vertices().reshape((-1, 3), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(3, "cgal");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(12, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

    def test_cgal_tet(self):
        if not PyConvexHull.ConvexHullEngine.supports("cgal"):
            return;

        mesh = self.load_mesh("tet.msh");
        points = mesh.get_vertices().reshape((-1, 3), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(3, "cgal");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(4, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

    def test_cgal_square(self):
        if not PyConvexHull.ConvexHullEngine.supports("cgal"):
            return;

        mesh = self.load_mesh("square_2d.obj");
        points = mesh.get_vertices().reshape((-1, 2), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(2, "cgal");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(4, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

    def test_qhull_cube(self):
        if not PyConvexHull.ConvexHullEngine.supports("qhull"):
            return;

        mesh = self.load_mesh("cube.msh");
        points = mesh.get_vertices().reshape((-1, 3), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(3, "qhull");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(12, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

    def test_qhull_tet(self):
        if not PyConvexHull.ConvexHullEngine.supports("qhull"):
            return;

        mesh = self.load_mesh("tet.msh");
        points = mesh.get_vertices().reshape((-1, 3), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(3, "qhull");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(4, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

    def test_qhull_square(self):
        if not PyConvexHull.ConvexHullEngine.supports("qhull"):
            return;

        mesh = self.load_mesh("square_2d.obj");
        points = mesh.get_vertices().reshape((-1, 2), order="C");

        engine = PyConvexHull.ConvexHullEngine.create(2, "qhull");
        engine.run(points);

        vertices = engine.get_vertices();
        faces = engine.get_faces();

        self.assertTupleEqual(points.shape, vertices.shape);
        self.assertEqual(4, len(faces));
        self.assertBBoxEqual(points, vertices);
        self.assertPositiveVolume(vertices, faces);

