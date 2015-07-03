#!python

"""
Extract vertices and normals from a surface mesh and store it as a point cloud
mesh.  The output file should has extension .ply so it works with Poisson
reconstruction code by Michael Misha Kazhdan
<http://www.cs.jhu.edu/~misha/Code/PoissonRecon/Version6.13/>
"""

import argparse
import pymesh
import numpy as np
from pymesh import load_mesh, save_mesh, form_mesh

def parse_args():
    parser = argparse.ArgumentParser(
            description=__doc__);
    parser.add_argument("input_mesh", help="Input mesh");
    parser.add_argument("output_mesh", help="Output point cloud");
    return parser.parse_args();

def main():
    args = parse_args();
    in_mesh = load_mesh(args.input_mesh);
    in_mesh.add_attribute("vertex_normal");
    v_normals = in_mesh.get_vertex_attribute("vertex_normal");

    out_mesh = form_mesh(in_mesh.vertices, np.zeros((0, 3), dtype=int));
    out_mesh.add_attribute("nx");
    out_mesh.add_attribute("ny");
    out_mesh.add_attribute("nz");
    out_mesh.set_attribute("nx", v_normals[:,0].ravel());
    out_mesh.set_attribute("ny", v_normals[:,1].ravel());
    out_mesh.set_attribute("nz", v_normals[:,2].ravel());

    save_mesh(args.output_mesh, out_mesh, "nx", "ny", "nz");

if __name__ == "__main__":
    main();
