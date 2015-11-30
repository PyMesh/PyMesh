from pymesh.TestCase import TestCase
from pymesh import CSGTree
from pymesh.meshutils import generate_box_mesh
from pymesh.misc import Quaternion

import numpy as np

class CSGTreeTest(TestCase):
    def test_single_mesh(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
        tree = CSGTree({"mesh": mesh});
        self.assert_array_equal(mesh.vertices, tree.vertices);
        self.assert_array_equal(mesh.faces, tree.faces);
