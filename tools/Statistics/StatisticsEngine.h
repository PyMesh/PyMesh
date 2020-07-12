/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "MeshStatistics.h"


namespace PyMesh {

class StatisticsEngine {
    public:
        typedef std::shared_ptr<StatisticsEngine> Ptr;
        static Ptr create(const std::string& engine_name);
        static bool supports(const std::string& engine_name);
        static std::vector<std::string> get_available_engines();

    public:
        virtual ~StatisticsEngine() {}

    public:
        void set_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices = vertices;
            m_faces = faces;
            convert_mesh_to_native_format();
        }

        const MatrixFr get_vertices() const { return m_vertices; }
        const MatrixIr get_faces() const { return m_faces; }

    public:
        virtual MeshStatistics compute_statistics() {
            throw NotImplementedError("Statistics computations are not implemented");
        }

    protected:
        virtual void convert_mesh_to_native_format() {}

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};

}
