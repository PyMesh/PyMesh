#include "AABBTiler.h"

#include <iostream>
#include <list>

namespace AABBTilerHelper {
    std::vector<VectorI> enumerate(const VectorI& repetitions) {
        std::vector<VectorI> result;
        const size_t dim = repetitions.size();
        if (dim == 2) {
            for (size_t i=0; i<repetitions[0]; i++) {
                for (size_t j=0; j<repetitions[1]; j++) {
                    result.push_back(Vector2I(i,j));
                }
            }
        } else if (dim == 3) {
            for (size_t i=0; i<repetitions[0]; i++) {
                for (size_t j=0; j<repetitions[1]; j++) {
                    for (size_t k=0; k<repetitions[2]; k++) {
                        result.push_back(Vector3I(i,j,k));
                    }
                }
            }
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
        }

        return result;
    }

    template<typename T>
    void run_compatibility_check(size_t dim, const T& data) {
        if (data.size() != dim) {
            std::stringstream err_msg;
            err_msg << "Imcompatible data dimention: " << data.size() << std::endl;
            err_msg << "Expect " << dim << std::endl;
            throw RuntimeError(err_msg.str());
        }
    }

    std::list<std::function<MatrixFr(const MatrixFr&)> > get_tiling_operators(
            const VectorF& ref_pt,
            const VectorF& cell_size,
            const std::vector<VectorI>& indices) {

        std::list<std::function<MatrixFr(const MatrixFr&)> > operators;
        for (auto index : indices) {
            VectorF cur_pt = cell_size.cwiseProduct(index.cast<Float>());
            VectorF offset = cur_pt - ref_pt;
            operators.push_back(
                    [=] (const MatrixFr& vertices) {
                        MatrixFr result(vertices);
                        result.rowwise() += offset.transpose();
                        return result;
                    });
        }
        return operators;
    }
}

using namespace AABBTilerHelper;

AABBTiler::AABBTiler(
        WireNetwork::Ptr unit_wire_network,
        const VectorF& bbox_min,
        const VectorF& bbox_max,
        const VectorI& repetitions)
: TilerEngine(unit_wire_network),
    m_bbox_min(bbox_min),
    m_bbox_max(bbox_max),
    m_repetitions(repetitions) {
        const size_t dim = m_unit_wire_network->get_dim();
        run_compatibility_check(dim, m_bbox_min);
        run_compatibility_check(dim, m_bbox_max);
        run_compatibility_check(dim, m_repetitions);
}

WireNetwork::Ptr AABBTiler::tile() {
    const size_t dim = m_unit_wire_network->get_dim();

    VectorF cell_size = (m_bbox_max - m_bbox_min).cwiseQuotient(
            m_repetitions.cast<Float>());
    normalize_unit_wire(cell_size);

    std::vector<VectorI> indices = enumerate(m_repetitions);
    const size_t num_repetitions = indices.size();
    MatrixFr tiled_vertices = tile_vertices(
            get_tiling_operators(
            m_unit_wire_network->get_bbox_min(),
            cell_size, indices));
    MatrixIr tiled_edges = tile_edges(num_repetitions);

    WireNetwork::Ptr tiled_network =
        WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_repetitions);
    clean_up(*tiled_network);
    return tiled_network;
}
