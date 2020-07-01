import PyMesh
from .meshio import form_mesh

class triangle(PyMesh.triangle):
    """ Wrapper around `Shewchuk's triangle
    <https://www.cs.cmu.edu/~quake/triangle.html>`_.

    Attributes:
        points (:class:`numpy.ndarray`): 3D or 2D points to be triangulated.
            If points are embedded in 3D, they must be coplanar.

        segments (:class:`numpy.ndarray`): n by 2 matrix of indices into points.
            together ``points`` and ``segments`` defines a Planar Straight Line
            Graph (PSLG) that triangles accepts.

        triangles (:class:`numpy.ndarray`): m by 3 matrix of indices into
            points.  When ``segments`` is empty and ``triangles`` is non-empty,
            use triangle to refine the existing triangulation.

        holes (:class:`numpy.ndarray`): h by dim matrix of points representing
            hole points.  Alternatively, one can set ``auto_hole_detection`` to
            True to infer holes from the input PSLG's orientation.

        min_angle (``float``): Lower bound on angle in degrees.  Default is 20
            degress.  Setting ``min_angle`` > 20.7 will loose the theoretical
            guarentee of termination, although it often works fine in practice.
            However, setting ``min_angle`` > 34 tends to cause triangle to not
            terminate in practice.

        max_area (``float``): Max triangle area.  Default is unbounded.

        max_areas (:class:`numpy.ndarray`): Max area scalar field.  It should
            have the same length as triangles.  Not used by default.

        keep_convex_hull (``boolean``): Whether to keep all triangles inside of
            the convex hull.  Default is false.

        conforming_delaunay (``boolean``): Whether to enforce conforming
            Delaunay triangulation.  Default is false (i.e. use constrained
            Delaunay triangulation).

        exact_arithmetic (``boolean``): Whether to use exact predicates.
            Defeault is true.

        split_boundary (``boolean``): Whether to allow boundary to be split.
            Default is false.

        max_num_steiner_points (``int``): The maximum number of Steiner points.
            Default is -1 (i.e. unbounded).

        verbosity (``int``): How much info should triangle output?

            0. no output
            1. normal level of output
            2. verbose output
            3. vertex-by-vertex details
            4. you must be debugging the triangle code

        algorithm (``str``): The Delaunay triangulation algorithm to use.
            Choices are:

            * ``DIVIDE_AND_CONQUER``: Default.  Implementation of [1]_.
            * ``SWEEPLINE``: Fortune's sweep line algorithm [2]_.
            * ``INCREMENTAL``: Also from [1]_.

        auto_hole_detection (``boolean``): Whether to detect holes based on the
            orientation of PSLG using winding number.  Default is False.

        vertices (:class:`numpy.ndarray`): Vertices of the output triangulation.

        faces (:class:`numpy.ndarray`): Faces of the output triangulation.

        mesh (:class:`Mesh`): Output mesh.

        voronoi_vertices (:class:`numpy.ndarray`): Vertices of the output
            Voronoi diagram.  Only generated when no input segments and
            triangles are provided.

        voronoi_edges (:class:`numpy.ndarray`): Voronoi edges.  Negative index
            indicates infinity.

        regions (:class:`numpy.ndarray`): Per-triangle index of connected
            regions separated by segments.

    Example:

        >>> vertices = np.array([
        ...     [0.0, 0.0],
        ...     [1.0, 0.0],
        ...     [1.0, 1.0],
        ...     [0.0, 1.0],
        ...     ])
        >>> tri = pymesh.triangle()
        >>> tri.points = vertices
        >>> tri.max_area = 0.05
        >>> tri.split_boundary = False
        >>> tri.verbosity = 0
        >>> tri.run() # Execute triangle.
        >>> mesh = tri.mesh # output triangulation.

    References:

    .. [1] Leonidas J. Guibas and Jorge Stolfi, Primitives for the Manipulation
        of General Subdivisions and the Computation of Voronoi Diagrams, ACM
        Transactions on Graphics 4(2):74-123, April 1985.

    .. [2] Steven Fortune, A Sweepline Algorithm for Voronoi Diagrams,
        Algorithmica 2(2):153-174, 1987.

    """
    @property
    def mesh(self):
        return form_mesh(self.vertices, self.faces)

    @property
    def DIVIDE_AND_CONQUER(self):
        return Algorithm.DIVIDE_AND_CONQUER

    @property
    def SWEEPLINE(self):
        return Algorithm.SWEEPLINE

    @property
    def INCREMENTAL(self):
        return Algorithm.INCREMENTAL

    def __setattr__(self, key, value):
        if not hasattr(self, key):
            # Need to disable pesky dynamic attributes so that we can catch typo
            # early on.
            raise AttributeError(
                    "Attribute '{}' does not exists!".format(key))
        else:
            PyMesh.triangle.__setattr__(self, key, value)



