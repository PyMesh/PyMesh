import numpy as np
from numpy.linalg import norm
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
import PyCGAL

class AABBTreeTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        return factory.create_shared();

    def create_aabb_tree(self, mesh):
        vertices = mesh.get_vertices().reshape((-1, mesh.get_dim()), order="C");
        faces = mesh.get_faces().reshape((-1, mesh.get_vertex_per_face()), order="C");
        return PyCGAL.AABBTree(vertices, faces);

    def test_face_centroid(self):
        mesh = self.load_mesh("cube.obj");
        mesh.add_attribute("face_centroid");
        queries = mesh.get_attribute("face_centroid").reshape((-1, 3), order="C");
        aabb_tree = self.create_aabb_tree(mesh);
        distances, face_indices = aabb_tree.look_up(queries);
        self.assertAlmostEqual(0.0, np.max(distances));

    def test_cube(self):
        mesh = self.load_mesh("cube.obj");
        aabb_tree = self.create_aabb_tree(mesh);
        queries = np.array([
            [1.2, 1.0, 1.0],
            [1.0, 1.2, 1.0],
            [1.0, 1.0, 1.2],
            ]);
        sq_distances, face_indices, closest_pts = \
                aabb_tree.look_up_with_closest_points(queries);
        self.assertAlmostEqual(0.0, norm(sq_distances - 0.2**2));
        self.assertAlmostEqual(0.0, norm(closest_pts - 1.0));
