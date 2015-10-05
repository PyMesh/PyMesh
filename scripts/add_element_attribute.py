#!/usr/bin/env python

"""
Add per-element scalar field to mesh computed from a given formula.
"""

import argparse
import numpy as np
import parser
import re

import pymesh

class Formula:
    def __init__(self, mesh):
        self.mesh = mesh;
        self.field_parser = re.compile("\{\w+\}");
        self.__load_default_fields();

    def __load_default_fields(self):
        self.index = np.arange(self.mesh.num_elements, dtype=int);
        if self.mesh.num_voxels == 0:
            self.mesh.add_attribute("face_centroid");
            elem_centers = self.mesh.get_attribute("face_centroid");
            elem_centers = elem_centers.reshape(
                    (self.mesh.num_faces, self.mesh.dim));
        else:
            self.mesh.add_attribute("voxel_centroid");
            elem_centers = self.mesh.get_attribute("voxel_centroid");
            elem_centers = elem_centers.reshape(
                    (self.mesh.num_voxels, self.mesh.dim));
        bbox_min, bbox_max = self.mesh.bbox;
        self.x = elem_centers[:, 0].ravel();
        self.x_p = (self.x - bbox_min[0]) / (bbox_max[0] - bbox_min[0]);
        self.y = elem_centers[:, 1].ravel();
        self.y_p = (self.y - bbox_min[1]) / (bbox_max[1] - bbox_min[1]);
        if self.mesh.dim == 3:
            self.z = elem_centers[:, 2].ravel();
            self.z_p = (self.z - bbox_min[2]) / (bbox_max[2] - bbox_min[2]);

    def eval_formula(self, formula):
        self.formula = formula;
        field_names = self.field_parser.findall(formula);
        for name in field_names:
            field_name = name[1:-1];
            if hasattr(self, field_name):
                escaped_name = field_name;
            else:
                escaped_name = self.load_field(field_name);
            self.formula = self.formula.replace(name,
                    "self.{}".format(escaped_name));

        st = parser.expr(self.formula);
        code = st.compile("file.py");
        return eval(code);

    def load_field(self, name):
        if hasattr(self, name): return name;
        assert(self.mesh.has_attribute(name));
        value = self.mesh.get_attribute(name);
        escaped_name = name.replace(" ", "_")
        setattr(self, name, value);
        return escaped_name

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--name", help="attribute name");
    parser.add_argument("--formula", help="formula to compute new attribute");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    formula = Formula(mesh);
    values = formula.eval_formula(args.formula);
    if isinstance(values, list):
        values = np.array(values, dtype=float);
    elif isinstance(values, (int, float)):
        values = np.ones(mesh.num_elements, dtype=float) * values;
    mesh.add_attribute(args.name);
    mesh.set_attribute(args.name, values);

    pymesh.save_mesh(args.output_mesh, mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();
