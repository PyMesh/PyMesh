# This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou #
import os
import os.path
import unittest
import PyWires
import PyMesh

class WireTest(unittest.TestCase):
    def setUp(self):
        root_dir = os.environ.get("PYMESH_PATH");
        self.data_dir = os.path.join(root_dir, "tests/tools/Wires/data/");
        self.mesh_data_dir = os.path.join(root_dir, "tests/data/");

    def load_wire_file(self, filename):
        filename = os.path.join(self.data_dir, filename);
        return PyWires.WireNetwork.create(filename);

    def load_mesh(self, filename):
        filename = os.path.join(self.mesh_data_dir, filename);
        factory = PyMesh.MeshFactory();
        factory.load_file(filename);
        return factory.create_shared();
