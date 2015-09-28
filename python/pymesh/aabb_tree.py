import PyCGAL
import numpy as np

class AABBTree:
    def __init__(self):
        self.__raw_tree = None;

    def load_data(self, vertices, faces):
        self.__raw_tree = PyCGAL.AABBTree(vertices, faces);

    def load_mesh(self, mesh):
        self.__raw_tree = PyCGAL.AABBTree(mesh.vertices, mesh.faces);

    def look_up(self, pts):
        sq_dists, face_indices = \
                self.__raw_tree.look_up(pts);
        return sq_dists.squeeze(), face_indices.squeeze();

    def look_up_with_closest_points(self, pts):
        sq_dists, face_indices, closest_pts = \
                self.__raw_tree.look_up_with_closest_points(pts);
        return sq_dists.squeeze(), face_indices.squeeze(), closest_pts;


def distance_to_mesh(mesh, pts):
    """ Compute the distance from a set of points to a mesh.

    Args:
        mesh (:class:`Mesh`): A input mesh.
        pts (:class:`numpy.ndarray`): A :math:`N \\times dim` array of query
            points.

    Returns:
        Three values are returned.

            * ``squared_distances``: squared distances from each point to mesh.
            * ``face_indices``  : the closest face to each point.
            * ``closest_points``: the point on mesh that is closest to each
                                  query point.
    """

    tree = AABBTree();
    tree.load_mesh(mesh);
    squared_distances, face_indices, closest_points =\
            tree.look_up_with_closest_points(pts);
    return squared_distances, face_indices, closest_points;
