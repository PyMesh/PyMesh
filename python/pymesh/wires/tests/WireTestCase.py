import os.path

from pymesh.TestCase import TestCase
from pymesh.wires.WireNetwork import WireNetwork

class WireTestCase(TestCase):
    def setUp(self):
        super(WireTestCase, self).setUp();
        self.wire_dir = os.path.join(self.pymesh_path, "tests/tools/Wires/data");

    def load_wires(self, wire_file):
        if not os.path.isabs(wire_file):
            wire_file = os.path.join(self.wire_dir, wire_file);
        wire_network = WireNetwork();
        wire_network.load_from_file(wire_file);
        return wire_network;

    def save_wires(self, wire_network, wire_file):
        wire_network.write_to_file(wire_file);

    def find_file(self, filename):
        if not os.path.isabs(filename):
            wire_filename = os.path.join(self.wire_dir, filename);
            if os.path.exists(wire_filename):
                return wire_filename;

            data_filename = os.path.join(self.data_dir, filename);
            if os.path.exists(data_filename):
                return data_filename;

            raise IOError("Cannot find file: {}".format(filename));

        return filename;
