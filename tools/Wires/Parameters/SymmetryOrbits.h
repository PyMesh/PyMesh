/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * This class is dedicated to parsing the orbit file.
 */
class SymmetryOrbits {
    public:
        /**
         * Orbit file is in JSON format.  It has the following syntax:
         *   {
         *      "vertex_orbits": [
         *          [ #, #, #, ...],
         *          [ #, #, #, ...],
         *          ...
         *      ],
         *      "edge_orbits": [
         *          [ #, #, #, ...],
         *          [ #, #, #, ...],
         *          ...
         *      ]
         *   }
         */
        SymmetryOrbits(const std::string& orbit_file);

    public:
        size_t get_num_vertex_orbits() const { return m_vertex_orbits.size(); }
        const VectorI& get_vertex_orbit(size_t i) { return m_vertex_orbits.at(i); }

        size_t get_num_edge_orbits() const { return m_edge_orbits.size(); }
        const VectorI& get_edge_orbit(size_t i) { return m_edge_orbits.at(i); }

    private:
        std::vector<VectorI> m_vertex_orbits;
        std::vector<VectorI> m_edge_orbits;
};

}
