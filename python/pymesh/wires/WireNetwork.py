import numbers
import numpy as np
from numpy.linalg import norm

import PyMesh

class WireNetwork(object):
    """ Data structure for wire network.

    A wire network consists of a list of vertices and a list of edges.
    Thus, it is very similar to a graph except all vertices have their positions
    specified. Optionally, each vertex and edge could be associated with one or
    more attributes.

    Attributes:

        dim (``int``): The dimension of the embedding space of this wire network.
            Must be 2 or 3.

        vertices (:py:class:`numpy.ndarray`): A ``V`` by ``dim`` vertex matrix.
            One vertex per row.

        edges (:py:class:`numpy.ndarray`): A ``E`` by 2 edge matrix.
            One edge per row.

        num_vertices (``int``): The number of vertices (i.e. ``V``).

        num_edges (``int``): The number of edges (i.e. ``E``).

        bbox (:py:class:`numpy.ndarray`): A 2 by ``dim`` matrix with first and
            second rows being the minimum and maximum corners of the bounding
            box respectively.

        bbox_center (:py:class:`numpy.ndarray`): Center of the ``bbox``.

        centroid (:py:class:`numpy.ndarray`): Average of all ``vertices``.

        wire_lengths (:py:class:`numpy.ndarray`): An array of lengths of all
            edges.

        total_wire_length (``float``): Sum of ``wire_lengths``.

        attribute_names (:py:class:`list` of :py:class:`str`): The names of all
            defined attributes.
    """

    @classmethod
    def create_empty(cls):
        """ Handy factory method to create an empty wire network.
        """
        return cls()

    @classmethod
    def create_from_file(cls, wire_file):
        """ Handy factory method to load data from a file.
        """
        wire_network = cls()
        wire_network.load_from_file(wire_file)
        return wire_network

    @classmethod
    def create_from_data(cls, vertices, edges):
        """ Handy factory method to create wire network from ``vertices`` and
        ``edges``.

        Example:

            >>> vertices = np.array([
            ...     [0.0, 0.0, 0.0],
            ...     [1.0, 0.0, 0.0],
            ...     [1.0, 1.0, 0.0],
            ...     ])
            >>> edges = np.array([
            ...     [0, 1],
            ...     [0, 2],
            ...     [1, 2],
            ...     ])
            >>> wires = WireNetwork.create_from_data(vertices, edges)
        """
        wire_network = cls()
        wire_network.load(vertices, edges)
        return wire_network

    def __init__(self):
        """ Create empty a wire network.
        """
        self.raw_wires = PyMesh.WireNetwork.create_empty()
        self.__initialize_wires()

    def load(self, vertices, edges):
        """ Load vertices and edges from data.

        Args:
            vertices (:py:class:`numpy.ndarray`): :py:attr:`num_vertices` by
                :py:attr:`dim` array of vertex coordinates.
            faces (:py:class:`numpy.ndarray`): :py:attr:`num_edges` by 2 array
                of vertex indices.
        """
        self.raw_wires = PyMesh.WireNetwork.create_raw(vertices, edges)
        self.__initialize_wires()

    def load_from_file(self, wire_file):
        """ Load vertices and edges from a file.

        Args:
            wire_file (:py:class:`str`): Input wire file name.

        The file should have the following format::

            # This is a comment
            v x y z
            v x y z
            ...
            l i j # where i and j are vertex indices (starting from 1)
            l i j
            ...

        """
        self.raw_wires = PyMesh.WireNetwork.create(wire_file)
        self.__initialize_wires()

    def load_from_raw(self, raw_wires):
        """ Load vertex and edges from raw C++ wire data structure.
        """
        self.raw_wires = raw_wires
        self.__initialize_wires()

    def write_to_file(self, filename):
        """ Save the current wire network into a file.
        """
        self.raw_wires.write_to_file(filename)

    def scale(self, factors):
        """ Scale the wire network by factors

        Args:
            factors: scaling factors.  Scale uniformly if ``factors`` is a
                scalar.  If ``factors`` is an array, scale each dimension
                separately (dimension ``i`` is scaled by ``factors[i]``).
        """
        if isinstance(factors, numbers.Number):
            factors = np.ones(self.dim) * factors
        self.raw_wires.scale(factors)

    def offset(self, offset_vector):
        """ Offset vertices by per-vertex ``offset_vector``.

        Args:
            offset_vector (:py:class:`numpy.ndarray`): A :math:`N \times dim`
                matrix representing per-vertex offset vectors.
        """
        vertices = self.vertices + offset_vector
        self.vertices = vertices

    def center_at_origin(self):
        """ Translate the wire networks to have its center at the origin.
        """
        self.raw_wires.center_at_origin()

    def trim(self):
        """ Remove all hanging edges.
        e.g. edge with at least one vertex of valance <= 1
        """
        while np.any(self.vertex_valance <= 1):
            edge_to_keep = np.all(self.vertex_valance[self.edges] > 1,
                    axis=1).tolist()
            self.raw_wires.filter_edges(edge_to_keep)
            vertex_to_keep = [len(self.get_vertex_neighbors(i)) > 0 for i in
                    range(self.num_vertices)]
            self.raw_wires.filter_vertices(vertex_to_keep)

            self.__initialize_wires()
            if len(self.vertices) == 0:
                raise RuntimeError("Zero vertices left after trimming.")

    def filter_vertices(self, to_keep):
        """ Remove all vertices other than the ones marked with to_keep.
        Edges are updated accordingly.
        """
        self.raw_wires.filter_vertices(to_keep)
        self.__initialize_wires()

    def filter_edges(self, to_keep):
        """ Remove all edges unless marked with to keep.
        Vertices are left unchanged.
        """
        self.raw_wires.filter_edges(to_keep)
        self.__initialize_wires()

    def compute_symmetry_orbits(self):
        """ Compute the following symmetry orbits:

        * ``vertex_symmetry_orbit``: all vertices belonging to the same orbit
          can be mapped to each other by reflection with respect to planes
          orthogonal to axis.
        * ``vertex_cubic_symmetry_orbit``: all vertices belonging to the same
          orbit can be mapped to each other by reflection with respect to any
          symmetry planes of a perfect cube.
        * ``edge_symmetry_orbit``: all edges belonging to the same orbit can be
          mapped to each other by reflection with respect to planes orthogonal
          to axis.
        * ``edge_cubic_symmetry_orbit``: all edges belonging to the same orbit
          can be mapped to each other by reflection with respect to any symmetry
          planes of a perfect cube.

        All orbits are stored as attributes.
        """
        self.add_attribute("vertex_symmetry_orbit")
        self.add_attribute("vertex_cubic_symmetry_orbit")
        self.add_attribute("edge_symmetry_orbit")
        self.add_attribute("edge_cubic_symmetry_orbit")

    def get_attribute_names(self):
        """ Get the names of all defined attributes.
        """
        return self.raw_wires.get_attribute_names()

    def has_attribute(self, name):
        """ Check if an attribute exists.
        """
        return self.raw_wires.has_attribute(name)

    def add_attribute(self, name, value=None, vertex_wise=True):
        """ Add a new attirbute.

        Args:
            name (``str``): Attribute name.

            value (:py:class:`numpy.ndarray`): N by d matrix of attribute
                values (one row per vertex or per edge).  Default is None to
                represent uninitialized attribute value.

            vertex_wise (``bool``): Whether this attribute is assigned to
                vertices or edges.
        """
        if not self.has_attribute(name):
            self.raw_wires.add_attribute(name, vertex_wise)
        if value is not None:
            self.raw_wires.set_attribute(name, value)

    def get_attribute(self, name):
        """ Get the value of an attribute.
        """
        assert(self.has_attribute(name))
        return self.raw_wires.get_attribute(name).squeeze()

    def is_vertex_attribute(self, name):
        """ Returns true if ``name`` is a per-vertex attribute.
        """
        assert(self.has_attribute(name))
        return self.raw_wires.is_vertex_attribute(name)

    def set_attribute(self, name, value):
        """ Set the value of the attribute ``name`` to be ``value``.
        """
        assert(self.has_attribute(name))
        self.raw_wires.set_attribute(name, value)

    def get_vertex_neighbors(self, i):
        """ Returns a list of vertex indices which are connected to vertex ``i``
        via an edge.
        """
        if not self.raw_wires.with_connectivity():
            self.raw_wires.compute_connectivity()
        return self.raw_wires.get_vertex_neighbors(i).ravel()

    def __initialize_wires(self):
        self.raw_wires.compute_connectivity()
        if self.num_edges > 0:
            self.vertex_valance = np.array([
                    len(self.raw_wires.get_vertex_neighbors(i)) for i in
                    range(self.num_vertices) ], dtype=int)

    @property
    def dim(self):
        return self.raw_wires.get_dim()

    @property
    def num_vertices(self):
        return self.raw_wires.get_num_vertices()

    @property
    def num_edges(self):
        return self.raw_wires.get_num_edges()

    @property
    def vertices(self):
        return self.raw_wires.get_vertices()

    @vertices.setter
    def vertices(self, value):
        self.raw_wires.set_vertices(value)

    @property
    def edges(self):
        return self.raw_wires.get_edges()

    @edges.setter
    def edges(self, value):
        self.raw_wires.set_edges(value)
        self.__initialize_wires()

    @property
    def bbox(self):
        return (self.raw_wires.get_bbox_min().ravel(),
                self.raw_wires.get_bbox_max().ravel())

    @property
    def bbox_center(self):
        return self.raw_wires.center().ravel()

    @property
    def centroid(self):
        return np.average(self.vertices, axis=0)

    @property
    def total_wire_length(self):
        return np.sum(self.wire_lengths)

    @property
    def wire_lengths(self):
        return norm(
            self.vertices[self.edges[:,0]] -
            self.vertices[self.edges[:,1]], axis=1)

    @property
    def attribute_names(self):
        return self.raw_wires.get_attribute_names()

