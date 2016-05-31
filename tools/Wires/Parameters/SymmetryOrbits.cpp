/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SymmetryOrbits.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

typedef boost::property_tree::ptree PTree;

using namespace PyMesh;

SymmetryOrbits::SymmetryOrbits(const std::string& orbit_file) {
    PTree orbit_config;
    read_json(orbit_file, orbit_config);
    auto v_orbit_config = orbit_config.find("vertex_orbits");
    if (v_orbit_config != orbit_config.not_found()) {
        for (const auto& child : v_orbit_config->second) {
            VectorI orbit(child.second.size());
            size_t count = 0;
            for (auto val : child.second) {
                orbit[count] = val.second.get_value<int>();
                count++;
            }
            m_vertex_orbits.push_back(orbit);
        }
    }

    auto e_orbit_config = orbit_config.find("edge_orbits");
    if (e_orbit_config != orbit_config.not_found()) {
        for (const auto& child : e_orbit_config->second) {
            VectorI orbit(child.second.size());
            size_t count=0;
            for (auto val : child.second) {
                orbit[count] = val.second.get_value<int>();
                count++;
            }
            m_edge_orbits.push_back(orbit);
        }
    }
}
