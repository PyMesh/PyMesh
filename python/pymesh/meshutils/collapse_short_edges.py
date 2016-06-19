import logging
import math
import numpy as np

from .. import timethis
from ..meshio import form_mesh

from PyMeshUtils import ShortEdgeRemoval, IsolatedVertexRemoval, FinFaceRemoval

class _EdgeCollapser(object):
    """ Wrapper class for C++ ShortEdgeRemoval class.

    Attributes:
        input_mesh: The input mesh.
        vertices (2D array): the output vertices.
        faces (2D array): the output faces.

    Examples:
        A common usage::

            collapser = _EdgeCollapser.create(mesh);
            collapser.keep_features();
            collapser.collapse(0.1, None);
            print(collapser.vertices);
            print(collapser.faces);
    """
    @classmethod
    def create(cls, mesh):
        return _EdgeCollapser(mesh);

    @classmethod
    def create_raw(cls, vertices, faces):
        mesh = form_mesh(vertices, faces);
        return _EdgeCollapser(mesh);

    def __init__(self, mesh):
        self.input_mesh = mesh;
        if self.input_mesh.vertex_per_face != 3:
            raise RuntimeError("Only triangle mesh is supported!  "
                    "Input has {} vertices per face".format(
                        self.input_mesh.vertex_per_face));
        self.logger = logging.getLogger(__name__);
        self.importance = None;

    @timethis
    def keep_features(self):
        """ Preserve sharp edges and boundaries.
        """
        if not self.input_mesh.has_attribute("vertex_dihedral_angle"):
            self.input_mesh.add_attribute("vertex_dihedral_angle");
        dihedral_angle = self.input_mesh.get_attribute("vertex_dihedral_angle");
        self.importance = np.round(dihedral_angle * 4 / math.pi).astype(int);

        # keep boundary.
        bd_vertices = self.input_mesh.boundary_vertices;
        self.importance[bd_vertices] = 10;

    @timethis
    def collapse(self, abs_threshold, rel_threshold):
        """ Note this method remove all edges with length less than threshold.
        This could result in a non-manifold mesh.
        """
        min_edge_length = abs_threshold;
        if rel_threshold is not None:
            ave_edge_len = self.__get_ave_edge_length()
            min_edge_length = rel_threshold  * ave_edge_len;
        self.logger.info("Minimum edge threshold: {:.3}".format(min_edge_length));

        num_collapsed = self.__collapse_C(min_edge_length);
        self.logger.info("{} edges collapsed".format(num_collapsed));

        self.__remove_fin_faces();
        self.__remove_isolated_vertices();

        return num_collapsed;

    def __get_ave_edge_length(self):
        if not self.input_mesh.has_attribute("edge_length"):
            self.input_mesh.add_attribute("edge_length");
        edge_lengths = self.input_mesh.get_attribute("edge_length");
        return np.mean(edge_lengths);

    @timethis
    def __collapse_C(self, min_edge_length):
        collapser = ShortEdgeRemoval(
                self.input_mesh.vertices, self.input_mesh.faces);
        if self.importance is not None:
            if len(self.importance) != self.input_mesh.num_vertices:
                raise RuntimeError("Invalid importance size!");
            collapser.set_importance(self.importance);
        num_collapsed = collapser.run(min_edge_length);
        self.vertices = collapser.get_vertices();
        self.faces = collapser.get_faces();
        self.face_index_map = collapser.get_face_indices().ravel();
        return num_collapsed;

    @timethis
    def __remove_isolated_vertices(self):
        remover = IsolatedVertexRemoval(self.vertices, self.faces);
        remover.run();
        self.vertices = remover.get_vertices();
        self.faces = remover.get_faces();

    @timethis
    def __remove_fin_faces(self):
        remover = FinFaceRemoval(self.vertices, self.faces);
        remover.run();
        self.vertices = remover.get_vertices();
        self.faces = remover.get_faces();

def collapse_short_edges_raw(vertices, faces, abs_threshold=0.0,
        rel_threshold=None, preserve_feature=False):
    """ Convenient function for collapsing short edges.

    Args:
        vertices (``numpy.ndarray``): Vertex array. One vertex per row.
        faces (``numpy.ndarray``): Face array. One face per row.
        abs_threshold (``float``): (optional) All edge with length below or
            equal to this threshold will be collapsed.  This value is ignored
            if ``rel_thresold`` is not ``None``.
        rel_threashold (``float``): (optional) Relative edge length threshold
            based on average edge length.  e.g. ``rel_threshold=0.1`` means all
            edges with length less than ``0.1 * ave_edge_length`` will be collapsed.
        preserve_feature (``bool``): True if shape features should be preserved.
            Default is false.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array. One vertex per row.
            * ``output_faces``: Output face array. One face per row.
            * ``information``: A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``num_edge_collapsed``: Number of edge collapsed.
            * ``source_face_index``: An array tracks the source of each output face.
              That is face ``i`` of the ``output_faces`` comes from face
              ``source_face_index[i]`` of the input faces.
    """
    collapser = _EdgeCollapser.create_raw(vertices, faces);
    if preserve_feature:
        collapser.keep_features();
    num_collapsed = collapser.collapse(abs_threshold, rel_threshold);
    info = {
            "num_edge_collapsed": num_collapsed,
            "source_face_index": collapser.face_index_map
            }
    return collapser.vertices, collapser.faces, info;

def collapse_short_edges(mesh,
        abs_threshold=0.0, rel_threshold=None, preserve_feature=False):
    """ Wrapper function of :func:`collapse_short_edges_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        abs_threshold (``float``): (optional) All edge with length below or
            equal to this threshold will be collapsed.  This value is ignored
            if ``rel_thresold`` is not ``None``.
        rel_threashold (``float``): (optional) Relative edge length threshold
            based on average edge length.  e.g. ``rel_threshold=0.1`` means all
            edges with length less than ``0.1 * ave_edge_length`` will be collapsed.
        preserve_feature (``bool``): True if shape features should be preserved.
            Default is false.

    Returns:
        2 values are returned.

            * ``output_Mesh`` (:class:`Mesh`): Output mesh.
            * ``information`` (:class:`dict`): A ``dict`` of additional informations.

        The following attribute are defined:

            * ``face_sources``: The index of input source face of each output face.

        The following fields are defined in ``information``:

            * ``num_edge_collapsed``: Number of edge collapsed.
    """
    vertices, faces, info = collapse_short_edges_raw(mesh.vertices, mesh.faces,
            abs_threshold, rel_threshold, preserve_feature);
    result = form_mesh(vertices, faces);
    result.add_attribute("face_sources")
    result.set_attribute("face_sources", info["source_face_index"]);
    del info["source_face_index"]
    return result, info;

