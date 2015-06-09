#!python

"""
Compute the outer hull of the input mesh.
"""

import argparse
import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--engine", choices=["auto", "igl"], default="auto");
    parser.add_argument("--interior", "-I", help="save inteior mesh",
            default=None, metavar="filename");
    parser.add_argument("--remove-duplicated-faces", action="store_true",
            help="Remove duplicated faces in outer hull");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.meshio.load_mesh(args.input_mesh);

    outer_hull, interior = pymesh.compute_outer_hull(mesh, engine=args.engine,
            with_interior=True);
    pymesh.meshio.save_mesh(args.output_mesh, outer_hull,
            *outer_hull.get_attribute_names());

    if args.interior is not None:
        pymesh.meshio.save_mesh(args.interior, interior);


if __name__ == "__main__":
    main();
