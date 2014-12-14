#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Mesh.h>
#include <Wires/WireNetwork/WireNetwork.h>

#include "ParameterCommon.h"
#include "ThicknessParameters.h"
#include "OffsetParameters.h"

class ParameterManager {
    public:
        typedef ParameterCommon::TargetType TargetType;
        typedef ParameterCommon::Variables Variables;
        typedef std::shared_ptr<ParameterManager> Ptr;

    public:
        /**
         * Create default parameter manager with all parameters initialized with
         * default values.
         */
        static Ptr create(
                WireNetwork::Ptr wire_network,
                Float default_thickness = 0.5,
                TargetType thickness_type = ParameterCommon::VERTEX);

        /**
         * Create parameter manager that contains 0 parameters.
         */
        static Ptr create_empty_manager(WireNetwork::Ptr wire_network,
                Float default_thickness = 0.5);

        /**
         * Create parameter manager with parameter specified by orbit files and
         * modifier files.
         */
        static Ptr create_from_setting_file(WireNetwork::Ptr wire_network,
                Float default_thickness,
                const std::string& orbit_file,
                const std::string& modifier_file);

    public:
        ParameterManager(WireNetwork::Ptr wire_network, Float default_thickness);

    public:
        const WireNetwork::Ptr get_wire_network() const {
            return m_wire_network;
        }

        void set_wire_network(WireNetwork::Ptr wire_network) {
            m_wire_network = wire_network;
            for (auto& param : m_thickness_params)
                param->set_wire_network(m_wire_network);
            for (auto& param : m_offset_params)
                param->set_wire_network(m_wire_network);
        }

        Float get_default_thickness() const {
            return m_thickness_params.get_default();
        }

        void set_default_thickness(Float thickness) {
            m_thickness_params.set_default(thickness);
        }

        const ThicknessParameters& get_thickness_params() const { return m_thickness_params; }
        ThicknessParameters& get_thickness_params() { return m_thickness_params; }

        const OffsetParameters& get_offset_params() const { return m_offset_params; }
        OffsetParameters& get_offset_params() { return m_offset_params; }

    public:
        // The following methods are mostly used for parameter sweep
        size_t get_num_dofs() const;
        size_t get_num_thickness_dofs() const;
        size_t get_num_offset_dofs() const;
        VectorF get_dofs() const;
        void set_dofs(const VectorF& values);
        std::vector<MatrixFr> compute_shape_velocity(Mesh::Ptr mesh);
        VectorI get_thickness_dof_map() const;
        MatrixIr get_offset_dof_map() const;

        // The following methods are used for converting parameter into
        // thickness and offset values per vertex/edge.
        VectorF evaluate_thickness(const Variables& vars);
        MatrixFr evaluate_offset(const Variables& vars);

        // The following methods are used for adding parameters in code.
        TargetType get_thickness_type() const {
            return m_thickness_params.get_type();
        }
        void set_thickness_type(TargetType type);
        void add_thickness_parameter(const VectorI& roi,
                const std::string& formula, Float value);

        TargetType get_offset_type() const {
            return m_offset_params.get_type();
        }
        void set_offset_type(TargetType type);
        void add_offset_parameter(const VectorI& roi,
                const std::string& formula, Float value, size_t axis);
        void add_custom_offset_parameter(const VectorI& roi,
                const std::string& formula, Float value,
                const MatrixFr& custom_offset);

    private:
        WireNetwork::Ptr m_wire_network;
        ThicknessParameters m_thickness_params;
        OffsetParameters m_offset_params;

};
