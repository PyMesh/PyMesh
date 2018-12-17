import PyMesh

from .wires import WireNetwork

class Arrangement2(PyMesh.Arrangement2):
    """ 2D arrangement from CGAL
    """

    @property
    def wire_network(self):
        return WireNetwork.create_from_data(self.vertices, self.edges);
