from ..meshio import form_mesh
import numpy as np
import logging

def merge_meshes(input_meshes):
    """ Merge multiple meshes into a single mesh.

    Args:
        input_meshes (``list``): a list of input :class:`Mesh` objects.

    Returns:

        ``Mesh``: An mesh consists of all vertices, faces and voxels
        from ``input_meshes``.  The following mesh attributes are defined:

        * ``vertex_sources``: Indices of source vertices from the input mesh.
        * ``face_sources``: Indices of source faces from the input mesh if the
          output contains at least 1 face.
        * ``voxel_sources``: Indices of source voxels from the input mesh if the
          output contains at least 1 voxel.
    """
    logger = logging.getLogger(__name__);
    vertices = [];
    faces = [];
    voxels = [];
    vertex_count = 0;

    vertex_sources = [];
    face_sources = [];
    voxel_sources = [];

    for i,mesh in enumerate(input_meshes):
        vertices.append(mesh.vertices);
        vertex_sources.append(np.ones(mesh.num_vertices) * i);
        if mesh.num_faces > 0:
            faces.append(mesh.faces + vertex_count);
            face_sources.append(np.ones(mesh.num_faces) * i);
        if mesh.num_voxels > 0:
            voxels.append(mesh.voxels + vertex_count);
            voxel_sources.append(np.ones(mesh.num_voxels) * i);
        vertex_count += mesh.num_vertices;

    if len(vertices) > 0:
        vertices = np.vstack(vertices);
        vertex_sources = np.concatenate(vertex_sources);
    else:
        vertices = np.zeros((0, 3), dtype=float);
        vertex_sources = np.array([]);

    if len(faces) > 0:
        faces = np.vstack(faces);
        face_sources = np.concatenate(face_sources);
    else:
        faces = np.zeros((0, 3), dtype=int)
        face_sources = np.array([]);

    if len(voxels) > 0 and len(voxels) == len(input_meshes):
        voxels = np.vstack(voxels);
        voxel_sources = np.concatenate(voxel_sources);
    else:
        # Not all input meshes contains voxels.  So the merged mesh will not be
        # a valid volume representation.  It is probably base to drop all
        # voxels.
        if (len(voxels) > 0):
            logger.warning("Not all input meshes represent a volume, "
                    "so dropping all voxels.");

        voxels = np.zeros((0, 4), dtype=int);
        voxel_sources = np.array([]);

    output_mesh = form_mesh(vertices, faces, voxels);

    output_mesh.add_attribute("vertex_sources");
    output_mesh.set_attribute("vertex_sources", vertex_sources);
    if (len(face_sources) > 0):
        output_mesh.add_attribute("face_sources");
        output_mesh.set_attribute("face_sources", face_sources);
    if (len(voxel_sources) > 0):
        output_mesh.add_attribute("voxel_sources");
        output_mesh.set_attribute("voxel_sources", voxel_sources);

    return output_mesh;

