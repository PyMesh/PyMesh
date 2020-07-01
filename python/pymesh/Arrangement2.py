import PyMesh

from .wires import WireNetwork

class Arrangement2(PyMesh.Arrangement2):
    """ 2D arrangement from CGAL.

    Attributes:
        points (:class:`numpy.ndarray`): P by 2 array of input 2D points.

        segments (:class:`numpy.ndarray`): S by 2 array of input indices into
            ``points``, each row represent a line segment.

        vertices (:class:`numpy.ndarry`): Output vertices.

        edges (:class:`numpy.ndarray`): Output edges.

        wire_network (:class:`WireNetwork`): 2D output wirenetwork created from
            `vertices` and `edges`.

    Example:
        >>> points = np.array([
        ...     [0.0, 0.0],
        ...     [1.0, 0.0],
        ...     [1.0, 1.0],
        ...     [0.0, 1.0],
        ...     ])
        >>> segments = np.array([
        ...     [0, 2],
        ...     [1, 3],
        ...     ])
        >>> arrangement = pymesh.Arrangement2()
        >>> arrangement.points = points
        >>> arrangement.segments = segments
        >>> arrangement.run()
        >>> wire_network = arrangement.wire_network
    """

    @property
    def wire_network(self):
        return WireNetwork.create_from_data(self.vertices, self.edges)
