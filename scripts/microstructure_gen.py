#!/usr/bin/env python

"""
Tile a given wire network pattern according to a grid or hex mesh.
"""

import argparse
import json
import numpy as np
import os.path

import pymesh
from pymesh.wires import WireNetwork, Tiler, Parameters, Inflator

from find_file import find_file

def parse_config_file(config_file):
    """ syntax:
    {
        # opitional options:
        "trim": bool,
        "periodic": bool,
        "subdiv": #,
        "subdiv_method": "simple" or "loop"
        "rel_geometry_correction": [#, #, #],
        "abs_geometry_correction": [#, #, #],
        "geometry_correction_cap": #,
        "geometry_spread": #
        "geometry_correction_lookup": filename,
        "output_wire_network": filename,

        # options for specifying parameters
        "thickness": float,
        "modifier_file": modifier_file,
        "dof_file": dof_file,

        # options needed by guide bbox
        "wire_network": single_cell_wire_network,
        "bbox_min": [min_x, min_y, min_z],
        "bbox_max": [max_x, max_y, max_z],
        "repeats": [x_reps, y_reps, z_reps],

        # options needed by guide mesh
        "wire_network": single_cell_wire_network,
        "guide_mesh": guide_mesh,
        "dof_type": "isotropic" | "orthotropic",
        "thickness_type": "vertex" | "edge"

        # options neede by mixed pattern tiling
        "guide_mesh": guide_mesh,
        "wire_list_file": wire_list_file,
    }
    """
    config_dir = os.path.dirname(config_file);
    with open(config_file, 'r') as fin:
        config = json.load(fin);

    def convert_to_abs_path(field_name):
        field = config[field_name];
        if isinstance(field, str):
            config[field_name] = str(find_file(field, config_dir));

    if "wire_network" in config:
        convert_to_abs_path("wire_network");
    if "guide_mesh" in config:
        convert_to_abs_path("guide_mesh");
    if "modifier_file" in config:
        convert_to_abs_path("modifier_file");
    if "dof_file" in config:
        convert_to_abs_path("dof_file");
    if "wire_list_file" in config:
        convert_to_abs_path("wire_list_file");
    if "geometry_correction_lookup" in config:
        convert_to_abs_path("geometry_correction_lookup");
    return config;

def load_wire(wire_file):
    network = WireNetwork();
    network.load_from_file(wire_file);
    return network;

def load_wires(wire_list_file):
    root_dir = os.path.dirname(wire_list_file);
    with open(wire_list_file, 'r') as fin:
        wire_files = [str(name.strip()) for name in fin];
    wire_files = [find_file(name, root_dir) for name in wire_files];
    wires = [load_wire(name) for name in wire_files];
    return wires;

def load_parameters(wire_network, config):
    parameters = Parameters(wire_network, config["thickness"]);
    if "modifier_file" in config:
        if "dof_file" in config:
            print("dof_file is shadowed by modifier file!");
        parameters.load_modifier_file(config["modifier_file"]);
    elif "dof_file" in config:
        parameters.load_dof_file(config["dof_file"]);
    return parameters;

def extract_options(config):
    options = {
            "trim": config.get("trim", False),
            "periodic": config.get("periodic", False),
            "subdiv": config.get("subdiv", 1),
            "subdiv_method": str(config.get("subdiv_method", "simple")),
            "rel_geometry_correction": config.get("rel_geometry_correction"),
            "abs_geometry_correction": config.get("abs_geometry_correction"),
            "geometry_correction_cap": config.get("geometry_correction_cap"),
            "geometry_spread": config.get("geometry_spread"),
            "geometry_correction_lookup": config.get("geometry_correction_lookup"),
            }
    return options;

def tile(config):
    if "guide_mesh" in config:
        if "wire_list_file" in config:
            return tile_with_mixed_patterns(config);
        else:
            return tile_with_guide_mesh(config);
    else:
        return tile_with_guide_box(config);

def tile_with_guide_box(config):
    options = extract_options(config);
    network = load_wire(str(config["wire_network"]));
    parameters = load_parameters(network, config);
    tiler = Tiler();
    tiler.set_base_pattern(network);
    tiler.tile_with_guide_bbox(
            config["bbox_min"][:network.dim],
            config["bbox_max"][:network.dim],
            config["repeats"][:network.dim],
            parameters);
    network = tiler.wire_network;

    if config.get("trim", False):
        network.trim();
    if "output_wire_network" in config:
        network.write_to_file(config["output_wire_network"]);

    inflator = Inflator(network);
    inflator.subdivide_order = options["subdiv"];
    inflator.subdivide_method = options["subdiv_method"];
    inflator.inflate(network.get_attribute("thickness").ravel(),
            parameters.per_vertex_thickness);
    return inflator.mesh;

def tile_with_guide_mesh(config):
    options = extract_options(config);
    network = load_wire(str(config["wire_network"]));
    parameters = load_parameters(network, config);
    guide_mesh = pymesh.load_mesh(config["guide_mesh"]);

    tiler = Tiler();
    tiler.set_base_pattern(network);
    tiler.tile_with_guide_mesh(guide_mesh, parameters);
    network = tiler.wire_network;

    if config.get("trim", False):
        network.trim();
    if "output_wire_network" in config:
        network.write_to_file(config["output_wire_network"]);

    inflator = Inflator(network);
    inflator.subdivide_order = options["subdiv"];
    inflator.subdivide_method = options["subdiv_method"];
    inflator.inflate(network.get_attribute("thickness").ravel(),
            parameters.per_vertex_thickness);
    return inflator.mesh;

def tile_with_mixed_patterns(config):
    options = extract_options(config);
    options["dof_type"] = str(config.get("dof_type", "isotropic"));
    options["thickness_type"] = str(config.get("thickness_type", "vertex"));
    networks = load_wires(str(config["wire_list_file"]));
    guide_mesh = pymesh.load_mesh(config["guide_mesh"]);
    per_vertex_thickness = options["thickness_type"] == "vertex";

    tiler = Tiler();
    tiler.tile_with_mixed_patterns(mesh,
            per_vertex_thickness, options["dof_type"] == "isotropic");
    network = tiler.wire_network;

    if config.get("trim", False):
        network.trim();
    if "output_wire_network" in config:
        network.write_to_file(config["output_wire_network"]);

    inflator = Inflator(network);
    inflator.subdivide_order = options["subdiv"];
    inflator.subdivide_method = options["subdiv_method"];
    inflator.inflate(network.get_attribute("thickness").ravel(),
            per_vertex_thickness);
    return inflator.mesh;

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("config_file", help="pattern configuration file.");
    parser.add_argument("output", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();
    config = parse_config_file(args.config_file);
    if args.output is not None:
        config["output"] = args.output;
    mesh = tile(config);
    pymesh.save_mesh(config["output"], mesh, *mesh.get_attribute_names());

if __name__ == "__main__":
    main();

