/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#pragma once
#ifdef WITH_CGAL

#include <Statistics/StatisticsEngine.h>
#include <Statistics/MeshStatistics.h>
#include "CGALTypes.h"


namespace PyMesh {

    class CGALStatisticsEngine : public StatisticsEngine {
        public:
            virtual ~CGALStatisticsEngine() = default;

        public:
            virtual MeshStatistics compute_statistics() override;

        protected:
            virtual void convert_mesh_to_native_format() override;

        private:
            Mesh m_mesh;
    };

}

#endif
