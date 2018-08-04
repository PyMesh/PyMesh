import PyMesh
import numpy as np

class AABBTree:
    def __init__(self):
        self.__raw_tree = None;

    def load_data(self, vertices, faces):
        self.__raw_tree = PyMesh.AABBTree(vertices, faces);

    def load_mesh(self, mesh):
        self.__raw_tree = PyMesh.AABBTree(mesh.vertices, mesh.faces);

    def look_up(self, pts):
        sq_dists, face_indices = \
                self.__raw_tree.look_up(pts);
        return sq_dists.squeeze(), face_indices.squeeze();

    def look_up_with_closest_points(self, pts):
        sq_dists, face_indices, closest_pts = \
                self.__raw_tree.look_up_with_closest_points(pts);
        return sq_dists.squeeze(), face_indices.squeeze(), closest_pts;

    def do_intersect_segments(self, vertices, edges):
        return self.__raw_tree.do_intersect_segments(vertices, edges).squeeze();

class BVH:
    available_engines = PyMesh.BVHEngine.available_engines;

    def __init__(self, engine_name="auto", dim=3):
        self.__raw_bvh = PyMesh.BVHEngine.create(engine_name, dim);

    def load_data(self, vertices, faces):
        self.__raw_bvh.set_mesh(vertices, faces);
        self.__raw_bvh.build();

    def load_mesh(self, mesh):
        self.__raw_bvh.set_mesh(mesh.vertices, mesh.faces);
        self.__raw_bvh.build();

    def lookup(self, pts):
        sq_dists, face_indices, closest_pts = self.__raw_bvh.lookup(pts);
        return sq_dists.squeeze(), face_indices.squeeze(), closest_pts;


def distance_to_mesh(mesh, pts, engine="auto"):
    """ Compute the distance from a set of points to a mesh.

    Args:
        mesh (:class:`Mesh`): A input mesh.
        pts (:class:`numpy.ndarray`): A :math:`N \\times dim` array of query
            points.
        engine (``string``): BVH engine name. Valid choices are "cgal",
            "geogram", "igl" if all dependencies are used. The default is
            "auto" where an available engine is automatically picked.

    Returns:
        Three values are returned.

            * ``squared_distances``: squared distances from each point to mesh.
            * ``face_indices``  : the closest face to each point.
            * ``closest_points``: the point on mesh that is closest to each
                                  query point.
    """

    bvh = BVH(engine, mesh.dim);
    bvh.load_mesh(mesh);
    squared_distances, face_indices, closest_points = bvh.lookup(pts);
    return squared_distances, face_indices, closest_points;

def do_intersect(mesh, nodes, elements):
    """ Check if each element intersects the mesh.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        nodes(:class:`numpy.ndarray`): The nodes of the elements.
        elements (:class:`numpy.ndarray`): Connectivity of the nodes.

    Returns:
        A list indicating if each element intersects the mesh.
    """
    tree = AABBTree();
    tree.load_mesh(mesh);

    assert(elements.ndim == 2);
    elem_type = elements.shape[1];
    if (elem_type == 2):
        r =tree.do_intersect_segments(nodes, elements) != 0;
    else:
        raise NotImplementedError(
                "AABB tree does not support element consisting of {} nodes"\
                        .format(elem_type));
    return r;


