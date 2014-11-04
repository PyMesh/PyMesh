#pragma once

#include <list>
#include <Wires/WireNetwork/WireNetwork.h>

class TilerEngine {
    public:
        TilerEngine(const WireNetwork& unit_wire_network)
            : m_unit_wire_network(unit_wire_network) {}
        virtual ~TilerEngine() {}

    public:
        virtual WireNetwork tile()=0;


    public:
        typedef std::list<std::function<MatrixFr(const MatrixFr&)> > FuncList;

    protected:
        MatrixFr tile_vertices(const FuncList& funcs);
        MatrixIr tile_edges(size_t num_repetitions);

        void normalize_unit_wire(const VectorF& cell_size);
        void update_attributes(WireNetwork& wire_network, size_t num_repetitions);

        void clean_up(WireNetwork& wire_network, Float tol=1e-6);
        void remove_duplicated_vertices(WireNetwork& wire_network, Float tol);
        void remove_duplicated_edges(WireNetwork& wire_network);

    protected:
        WireNetwork m_unit_wire_network;
};
