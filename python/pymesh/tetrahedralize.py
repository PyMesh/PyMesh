import PyMesh
from .meshio import form_mesh, load_mesh, save_mesh

import hashlib
import logging
import math
import numpy as np
import numpy.linalg
import os
import os.path
import tempfile
import subprocess
from time import time

def tetrahedralize(mesh,
        cell_size,
        radius_edge_ratio=2.0,
        facet_distance=-1.0,
        feature_angle=120,
        engine="auto",
        with_timing=False):
    """ Create a tetrahedral mesh from input triangle mesh.

    Arguments:
        mesh (:class:`Mesh`): Input triangular mesh.
        cell_size (``float``): Max radius of the circumscribed sphere of the output tet.
        radius_edge_ratio (``float``): Max radius of the circumscribed sphere to the
            shortest edge length of each tet.
        facet_distance (``float``): Upper bound on the distance from the
            circumcenter of a facet to the center of its "Delaunay ball", where
            a Delaunay ball is defined as the smallest circumscribed sphere
            with center on the surface of the domain.
        feature_angle (``float``): Angle threshold (in degrees) for feature extraction.
        engine (``string``): The tetrahedralization engine to use.  Valid options are:

            * ``auto``: default to tetgen
            * ``cgal``: `CGAL 3D mesh generation`_, using Polyhedron domain with
              auto feature extraction.
            * ``cgal_no_features``: `CGAL 3D mesh generation`_,
              using Polyhedron domain without feature extraction.
            * ``cgal_implicit``: `CGAL 3D mesh generation`_,
              using implicit domain with winding number as oracle.
            * ``tetgen``: `TetGen`_ from Hang Si.
            * ``quartet``: `Quartet`_ from Robert Bridson and Crawford Doran
            * ``delpsc``: `DelPSC`_ from Tamal K Dey , Joshua A. Levine, Andrew
              Slatton
            * ``vegafem``: Tet mesher provided by `VegaFEM`_ library.
            * ``mmg``: Implicit domain meshing from `MMG3D`_.
            * ``tetwild``: `TetWild`_ engine based on our Siggraph paper.
        with_timing (``boolean``): whether to output timing info.

    Returns:
        Tetrahedral mesh (and running time if `with_timing` is True).

    .. _`CGAL 3D mesh generation`: https://doc.cgal.org/latest/Mesh_3/index.html
    .. _`TetGen`: http://wias-berlin.de/software/tetgen/
    .. _`Quartet`: https://github.com/crawforddoran/quartet
    .. _`DelPSC`: http://web.cse.ohio-state.edu/~dey.8/delpsc.html
    .. _`VegaFEM`: http://run.usc.edu/vega/
    .. _`MMG3D`: https://www.mmgtools.org/
    .. _`TetWild`: https://github.com/Yixin-Hu/TetWild
    """
    logger = logging.getLogger(__name__)
    bbox_min, bbox_max = mesh.bbox
    bbox_diagonal = numpy.linalg.norm(bbox_max - bbox_min)
    if cell_size <= 0.0:
        cell_size = bbox_diagonal / 20.0 * math.sqrt(6) / 4.0
    logger.info("Cell size: {}".format(cell_size))
    if facet_distance <= 0.0:
        facet_distance = bbox_diagonal / 1000.0
    logger.info("Facet distance: {}".format(facet_distance))
    if radius_edge_ratio <= 0.0:
        logger.warning("Using default radius edge ratio.")
        radius_edge_ratio = 2.0
    logger.info("Max radius/edge ratio: {}".format(radius_edge_ratio))
    if feature_angle < 0.0:
        logger.warning("Using default feature angle.")
        feature_angle = 120.0
    logger.info("Feature angle: {}".format(feature_angle))

    if mesh.dim != 3:
        raise NotImplementedError("Tetrahedralization only works with 3D mesh")
    if mesh.vertex_per_face != 3:
        raise NotImplementedError("Only triangle mesh is supported for now")
    if engine == 'auto':
        engine = 'tetgen'

    if engine == 'delpsc':
        exec_name = "DelPSC"
        temp_dir = tempfile.gettempdir()
        md5 = hashlib.md5()
        md5.update(mesh.vertices)
        md5.update(mesh.faces)
        name = md5.hexdigest()
        temp_file = os.path.join(temp_dir, "{}.off".format(name))
        save_mesh(temp_file, mesh)
        cmd = exec_name
        # DelPSC parameters are specified as percentages of the smallest
        # dimension of the bbox.
        reference_len = np.amin(bbox_diagonal)
        cmd += " -mtr {}".format(cell_size / reference_len)
        cmd += " -max {}".format(cell_size /
                (0.75 * math.sqrt(2) * reference_len))
        if radius_edge_ratio > 0.0:
            cmd += " -ar {}".format(radius_edge_ratio)
        if feature_angle > 0.0:
            cmd += " -at {}".format(feature_angle)
        cmd += " {} {}".format(temp_file, os.path.join(temp_dir, name))
        if with_timing:
            start_time = time()
        logger.info(cmd)
        subprocess.check_call(cmd.split())
        if with_timing:
            finish_time = time()
            running_time = finish_time - start_time
        logger.info("DelPSC done.")

        outfile_tet = os.path.join(temp_dir, "{}_vol.tets".format(name))
        outfile_off = os.path.join(temp_dir, "{}_vol.off".format(name))
        assert(os.path.exists(outfile_tet))
        os.rename(outfile_tet, outfile_off)
        # DelPSC save tet mesh using OFF format, so PyMesh would be able to
        # parse it (albeit as a quad mesh).
        delpsc_mesh = load_mesh(outfile_off)
        vertices = delpsc_mesh.vertices
        voxels = delpsc_mesh.faces
        output_mesh = form_mesh(vertices, np.zeros((0, 3)), voxels)
        if with_timing:
            return output_mesh, running_time
        else:
            return output_mesh
    elif engine == "vegafem":
        exec_name = "tetMesher"
        temp_dir = tempfile.gettempdir()
        md5 = hashlib.md5()
        md5.update(mesh.vertices)
        md5.update(mesh.faces)
        name = md5.hexdigest()
        temp_file = os.path.join(temp_dir, "{}.obj".format(name))
        output_file = os.path.join(temp_dir, "{}.vega".format(name))
        save_mesh(temp_file, mesh)
        logger.warning("VEGA tet mesher ignores cell_size and radius_edge_ratio")
        cmd = "{} {} {}".format(exec_name, temp_file, output_file)
        if with_timing:
            start_time = time()
        subprocess.check_call(cmd.split())
        if with_timing:
            finish_time = time()
            running_time = finish_time - start_time
        logger.info("VegaFEM tetMesher done.")
        vega_mesh = load_mesh(output_file)
        if with_time:
            return vega_mesh, running_time
        else:
            return vega_mesh
    else:
        vertices = mesh.vertices.reshape((-1, 3), order="C")
        faces = mesh.faces.reshape((-1, 3), order="C").astype(int)

        engine = PyMesh.TetrahedralizationEngine.create(engine)
        engine.set_vertices(vertices)
        engine.set_faces(faces)
        engine.set_cell_radius_edge_ratio(radius_edge_ratio)

        engine.set_cell_size(cell_size)
        engine.set_facet_distance(facet_distance)
        engine.set_feature_angle(feature_angle)

        if with_timing:
            start_time = time()

        engine.run()

        if with_timing:
            finish_time = time()
            running_time = finish_time - start_time

        vertices = engine.get_vertices()
        faces = engine.get_faces()
        voxels = engine.get_voxels()

        output_mesh = form_mesh(vertices, faces, voxels)
        if with_timing:
            return output_mesh, running_time
        else:
            return output_mesh

