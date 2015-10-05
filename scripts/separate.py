#!/usr/bin/env python

"""
Separate/highlight the disconnected components of the input mesh.

Connectivity can be defined as sharing a vertex/edge or face using
the --connectivity-type option.
"""

import argparse
import numpy as np
from pymesh import load_mesh, save_mesh
from pymesh import separate_mesh
import os.path

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--force", help="force separation", action="store_true");
    parser.add_argument("--connectivity-type", "-c",
            choices=["auto", "vertex", "face", "voxel"],
            help="the meaning of connected", default="auto");
    parser.add_argument("--highlight", action="store_true",
            help="Highlight disconnected components");
    parser.add_argument("mesh_in", help="input mesh");
    parser.add_argument("mesh_out", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();

    mesh = load_mesh(args.mesh_in);
    comps = separate_mesh(mesh, args.connectivity_type);

    if args.highlight:
        if (args.connectivity_type == "face"):
            comp_indicator = np.zeros(mesh.num_faces);
        elif (args.connectivity_type == "voxel"):
            comp_indicator = np.zeros(mesh.num_voxels);
        elif (mesh.num_voxels > 0):
            comp_indicator = np.zeros(mesh.num_voxels);
        else:
            comp_indicator = np.zeros(mesh.num_faces);

        for i in range(len(comps)):
            elem_sources = comps[i].get_attribute("ori_elem_index")\
                    .ravel().astype(int);
            comp_indicator[elem_sources] = i;
        mesh.add_attribute("component_index");
        mesh.set_attribute("component_index", comp_indicator);
        save_mesh(args.mesh_out, mesh, *mesh.get_attribute_names());
    else:
        basename, ext = os.path.splitext(args.mesh_out);
        for i,comp in enumerate(comps):
            filename = "{}_cc{}{}".format(basename, i, ext);
            save_mesh(filename, comp, *comp.get_attribute_names());

if __name__ == "__main__":
    main();
