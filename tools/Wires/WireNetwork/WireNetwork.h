/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Wires/Attributes/WireAttributes.h>
#include "WireConnectivity.h"

namespace PyMesh {

class WireNetwork {
    public:
        typedef std::shared_ptr<WireNetwork> Ptr;
        static Ptr create(const std::string& wire_file);
        static Ptr create_raw(const MatrixFr& vertices, const MatrixIr& edges);

    public:
        WireNetwork();
        WireNetwork(const std::string& wire_file);
        WireNetwork(const MatrixFr& vertices, const MatrixIr& edges);

    public:
        size_t get_dim() const { return m_dim; }
        size_t get_num_vertices() const { return m_vertices.rows(); }
        size_t get_num_edges() const { return m_edges.rows(); }

        const MatrixFr& get_vertices() const { return m_vertices; }
        const MatrixIr& get_edges() const { return m_edges; }

        void set_vertices(const MatrixFr& vertices) {
            m_vertices = vertices;
            m_dim = m_vertices.cols();
            update_bbox();
        }

        void set_edges(const MatrixIr& edges) {
            m_edges = edges;
            m_connectivity.reset();
        }

        void scale(const VectorF& factors);
        void scale_fit(const VectorF& bbox_min, const VectorF& bbox_max);
        void translate(const VectorF& offset);
        void center_at_origin();
        VectorF center() const;
        VectorF get_bbox_min() const { return m_bbox_min; }
        VectorF get_bbox_max() const { return m_bbox_max; }

        void filter_vertices(const std::vector<bool>& to_keep);
        void filter_edges(const std::vector<bool>& to_keep);

        void write_to_file(const std::string& filename) const;

    public:
        bool has_attribute(const std::string& name) const {
            return m_attributes.has_attribute(name);
        }

        bool is_vertex_attribute(const std::string& name) const {
            return m_attributes.is_vertex_attribute(name);
        }

        const MatrixFr& get_attribute(const std::string& name) const {
            return m_attributes.get_attribute(name);
        }

        void add_attribute(const std::string& name,
                bool vertex_wise=true, bool auto_compute=true) const {
            m_attributes.add_attribute(*this, name, vertex_wise, auto_compute);
        }

        void set_attribute(const std::string& name, const MatrixFr& values) const {
            m_attributes.set_attribute(*this, name, values);
        }

        std::vector<std::string> get_attribute_names() const {
            return m_attributes.get_attribute_names();
        }

        void clear_attributes() {
            m_attributes.clear();
        }

    public:
        bool with_connectivity() const {
            return m_connectivity.is_available();
        }

        void compute_connectivity() {
            assert(m_connectivity.get_wire_network() == this);
            m_connectivity.compute();
        }

        VectorI get_vertex_neighbors(size_t i) const {
            return m_connectivity.get_vertex_neighbors(i);
        }

    public:
        /**
         * Valid print dir: X, Y, Z, YZ, ZX, XY, XYZ.
         */
        bool is_printable(const std::string& print_dir = "Z");

    private:
        void initialize();
        void initialize_connectivity();
        void update_bbox();

    private:
        size_t m_dim;
        MatrixFr m_vertices;
        MatrixIr m_edges;
        mutable WireAttributes m_attributes;
        WireConnectivity m_connectivity;

        VectorF m_bbox_min;
        VectorF m_bbox_max;
};

}
