/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SymmetryOrbits.h"

#include <fstream>
#include <nlohmann/json.hpp>

using namespace PyMesh;

SymmetryOrbits::SymmetryOrbits(const std::string& orbit_file) {
    nlohmann::json orbit_config;
    std::ifstream fin(orbit_file.c_str());
    fin >> orbit_config;
    fin.close();

    auto v_orbit_config = orbit_config.find("vertex_orbits");
    if (v_orbit_config != orbit_config.end()) {
        for (const auto& child : *v_orbit_config) {
            VectorI orbit(child.size());
            size_t count = 0;
            for (auto val : child) {
                orbit[count] = val.get<int>();
                count++;
            }
            m_vertex_orbits.push_back(orbit);
        }
    }

    auto e_orbit_config = orbit_config.find("edge_orbits");
    if (e_orbit_config != orbit_config.end()) {
        for (const auto& child : *e_orbit_config) {
            VectorI orbit(child.size());
            size_t count=0;
            for (auto val : child) {
                orbit[count] = val.get<int>();
                count++;
            }
            m_edge_orbits.push_back(orbit);
        }
    }
}
