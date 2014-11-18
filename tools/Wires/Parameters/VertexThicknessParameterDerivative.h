#pragma once

#include <vector>
#include "ParameterDerivative.h"

class VertexThicknessParameterDerivative : public ParameterDerivative {
    public:
        VertexThicknessParameterDerivative(
                Mesh::Ptr mesh, PatternParameter::Ptr param)
            : ParameterDerivative(mesh, param) { }
        virtual ~VertexThicknessParameterDerivative() {}

    public:
        virtual MatrixFr compute();

    protected:
        typedef std::vector<bool> BoolVector;

        void compute_derivative_on_edge(
                size_t wire_edge_index,
                size_t face_index,
                const BoolVector& in_roi,
                MatrixFr& derivative_v,
                VectorF& weights);

        void compute_derivative_on_vertex(
                size_t wire_edge_index,
                size_t face_index,
                const BoolVector& in_roi,
                MatrixFr& derivative_v,
                VectorF& weights);

};
