import PyMesh
from .meshio import form_mesh

class tetgen(PyMesh.tetgen):
    """ Wrapper around Si's `TetGen <http://wias-berlin.de/software/tetgen/>`_.

    All attributes, except ``vertices``, ``faces``, ``voxels`` and ``mesh``,
    are either input geometry or configuration parameters.

    Attributes:
        points (:class:`numpy.ndarray`): n by 3 list of points to be
            tetrahedralized.

        triangles (:class:`numpy.ndarray`): m by 3 matrix of indices into
            points.  Together, points and triangles defined PLC.

        tetrhaedra (:class:`numpy.ndarray`): t by 4 matrix of indices into
            points.  Used for refinement.

        point_markers (:class:`numpy.ndarray`): List of integer point markers of
            size n.  Point marker cannot be 0.

        point_weights (:class:`numpy.ndarray`): List of point weights.  Used for
            weight Delaunay tetrahedralization.

        triangle_marker (:class:`numpy.ndarray`): List of integer triangle
            markers of size t.

        split_boundary (``bool``): whether to split input boundary.  Default is
            true.

        max_radius_edge_ratio (``float``): Default is 2.0.

        min_dihedral_angle (``float``): Default is 0.0.

        coarsening (``bool``): Coarsening the input tet mesh.  Default is false.

        max_tet_volume (``float``): Default is unbounded.

        optimization_level (``int``): Ranges from 0 to 10.  Default is 2.

        max_num_steiner_points (``int``): Default is unbounded.

        coplanar_tolerance (``float``): Used for determine when 4 points are
            coplanar.  Default is 1e-8.

        exact_arithmetic (``bool``): Whether to use exact predicates.  Default
            is true.

        merge_coplanar (``bool``): Whether to merge coplanar faces and nearby
            vertices.  Default is true.

        weighted_delaunay (``bool``): Compute weighted Delaunay
            tetrahedralization instead of conforming Delaunay.  Default is
            false.  This option requires `point_weights`.

        keep_convex_hull (``bool``): Keep all tets within the convex hull.
            Default is false.

        verbosity (``int``): Verbosity level.  Ranges from 0 to 4:

            0. no output
            1. normal level of output
            2. verbose output
            3. more details
            4. you must be debugging the tetgen code

        vertices (:class:`numpy.ndarray`): Vertices of the output tet mesh.

        faces (:class:`numpy.ndarray`): Faces of the output tet mesh.

        voxels (:class:`numpy.ndarray`): Voxels of the output tet mesh.

        mesh (:class:`Mesh`): Output tet mesh.

    Example:

        >>> input_mesh = pymesh.generate_icosphere(1.0, [0.0, 0.0, 0.0])
        >>> tetgen = pymesh.tetgen()
        >>> tetgen.points = input_mesh.vertices # Input points.
        >>> tetgen.triangles = input_mesh.faces # Input triangles
        >>> tetgen.max_tet_volume = 0.01
        >>> tetgen.verbosity = 0
        >>> tetgen.run() # Execute tetgen
        >>> mesh = tetgen.mesh # Extract output tetrahedral mesh.

    """
    @property
    def mesh(self):
        return form_mesh(self.vertices, self.faces, self.voxels)

    def __setattr__(self, key, value):
        if not hasattr(self, key):
            # Need to disable pesky dynamic attributes so that we can catch typo
            # early on.
            raise AttributeError(
                    "Attribute '{}' does not exists!".format(key))
        else:
            PyMesh.tetgen.__setattr__(self, key, value)

