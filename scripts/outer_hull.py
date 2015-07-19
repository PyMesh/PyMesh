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
    mesh = pymesh.load_mesh(args.input_mesh);
    mesh = pymesh.remove_degenerated_triangles(mesh);

    outer_hull, interior = pymesh.compute_outer_hull(mesh, engine=args.engine,
            with_interior=True);
    if args.remove_duplicated_faces:
        new_outer_hull, info = pymesh.remove_duplicated_faces(outer_hull);
        for attr_name in outer_hull.get_attribute_names():
            attr = outer_hull.get_attribute(attr_name);
            if len(attr) % outer_hull.num_faces == 0:
                attr = attr.reshape((outer_hull.num_faces, -1), order="C");
                attr = attr[info["ori_face_index"]];
                new_outer_hull.add_attribute(attr_name);
                new_outer_hull.set_attribute(attr_name, attr.ravel(order="C"));
        outer_hull = new_outer_hull;

    pymesh.save_mesh(args.output_mesh, outer_hull,
            *outer_hull.get_attribute_names());

    if args.interior is not None:
        pymesh.save_mesh(args.interior, interior);


if __name__ == "__main__":
    main();
