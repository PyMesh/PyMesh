/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <list>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Parameters/ParameterManager.h>

namespace PyMesh {

class TilerEngine {
    public:
        TilerEngine(WireNetwork::Ptr unit_wire_network)
            : m_unit_wire_network(unit_wire_network) {
                m_params = ParameterManager::create_empty_manager(
                        m_unit_wire_network);
            }

        virtual ~TilerEngine() {}

    public:
        virtual WireNetwork::Ptr tile()=0;


    public:
        typedef std::function<MatrixFr(const MatrixFr&)> Func;
        typedef std::list<Func> FuncList;

        void with_parameters(ParameterManager::Ptr params);

    protected:
        MatrixFr tile_vertices(const FuncList& funcs);
        MatrixIr tile_edges(size_t num_repetitions);

        void normalize_unit_wire(const VectorF& cell_size);
        void update_attributes(WireNetwork& wire_network, size_t num_repetitions);

        void clean_up(WireNetwork& wire_network, Float tol=1e-6);
        void remove_duplicated_vertices(WireNetwork& wire_network, Float tol);
        void remove_duplicated_edges(WireNetwork& wire_network);

    protected:
        WireNetwork::Ptr m_unit_wire_network;
        ParameterManager::Ptr m_params;
};

}
