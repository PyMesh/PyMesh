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

def tetrahedralize(mesh, cell_size, radius_edge_ratio, engine="auto"):
    """
    Create a tetrahedral mesh from input triangle mesh.

    Arguments:
        mesh (:class:`Mesh`): Input triangular mesh.
        cell_size (``float``): Max radius of the circumscribed sphere of the output tet.
        radius_edge_ratio (``float``): Max radius of the circumscribed sphere to the
            shortest edge length of each tet.
        engine (``string``): The tetrahedralization engine to use.  Valid options are:
            * ``auto``: default to tetgen
            * ``cgal``: `CGAL 3D mesh generation
                <https://doc.cgal.org/latest/Mesh_3/index.html>`_
            * ``tetgen``: `TetGen from Hang Si
                <http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1>`_
            * ``quartet``: `Quartet from Robert Bridson and Crawford Doran
                <https://github.com/crawforddoran/quartet>`_
            * ``delpsc``: `DelPSC from Tamal K Dey , Joshua A. Levine, Andrew
                Slatton <http://web.cse.ohio-state.edu/~dey.8/delpsc.html>`_
            * ``vegafem``: `Tet mesher provided by VegaFEM library
                <http://run.usc.edu/vega/>`_

    Returns:
        Tetrahedral mesh.
    """
    logger = logging.getLogger(__name__);
    if cell_size <= 0.0:
        cell_size = numpy.linalg.norm(mesh.bbox[0] - mesh.bbox[1]) / 20.0;

    if mesh.dim != 3:
        raise NotImplementedError("Tetrahedralization only works with 3D mesh");
    if mesh.vertex_per_face != 3:
        raise NotImplementedError("Only triangle mesh is supported for now");
    if engine == 'auto':
        engine = 'tetgen';
    elif engine == 'delpsc':
        exec_name = "DelPSC";
        temp_dir = tempfile.gettempdir();
        md5 = hashlib.md5();
        md5.update(mesh.vertices);
        md5.update(mesh.faces);
        name = md5.hexdigest();
        temp_file = os.path.join(temp_dir, "{}.off".format(name));
        save_mesh(temp_file, mesh);
        cmd = exec_name;
        cmd += " -mtr {}".format(cell_size * 0.5);
        cmd += " -max {}".format(cell_size * 0.5 / (0.75 * math.sqrt(2)));
        if radius_edge_ratio >= 2.0:
            cmd += " -ar {}".format(radius_edge_ratio);
        cmd += " {} {}".format(temp_file, os.path.join(temp_dir, name));
        subprocess.check_call(cmd.split());
        logger.info("DelPSC done.");

        outfile_tet = os.path.join(temp_dir, "{}_vol.tets".format(name));
        outfile_off = os.path.join(temp_dir, "{}_vol.off".format(name));
        assert(os.path.exists(outfile_tet));
        os.rename(outfile_tet, outfile_off);
        # DelPSC save tet mesh using OFF format, so PyMesh would be able to
        # parse it (albeit as a quad mesh).
        delpsc_mesh = load_mesh(outfile_off);
        vertices = delpsc_mesh.vertices;
        voxels = delpsc_mesh.faces;
        return form_mesh(vertices, np.zeros((0, 3)), voxels);
    elif engine == "vegafem":
        exec_name = "tetMesher";
        temp_dir = tempfile.gettempdir();
        md5 = hashlib.md5();
        md5.update(mesh.vertices);
        md5.update(mesh.faces);
        name = md5.hexdigest();
        temp_file = os.path.join(temp_dir, "{}.obj".format(name));
        output_file = os.path.join(temp_dir, "{}.vega".format(name));
        save_mesh(temp_file, mesh);
        logger.warning("VEGA tet mesher ignores cell_size and radius_edge_ratio");
        cmd = "{} {} {}".format(exec_name, temp_file, output_file);
        subprocess.check_call(cmd.split());
        logger.info("VegaFEM tetMesher done.");
        vega_mesh = load_mesh(output_file);
        return vega_mesh;
    else:
        vertices = mesh.vertices.reshape((-1, 3), order="C");
        faces = mesh.faces.reshape((-1, 3), order="C").astype(int);

        engine = PyMesh.TetrahedralizationEngine.create(engine);
        engine.set_vertices(vertices);
        engine.set_faces(faces);
        if radius_edge_ratio < 2.0:
            engine.set_cell_radius_edge_ratio(radius_edge_ratio);
        else:
            logger.warning("Using default radius edge ratio.");

        engine.set_cell_size(cell_size);
        engine.run();

        vertices = engine.get_vertices();
        faces = engine.get_faces();
        voxels = engine.get_voxels();

        return form_mesh(vertices, faces, voxels);

