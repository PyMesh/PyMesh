/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Interfaces/PeriodicExploration.h>
#include <CGAL/AABBTree.h>

class PeriodicExplorationTest : public WireTest {
    protected:
        void ASSERT_ASSENDING_DIRECTION(const MatrixFr& delta_y, const MatrixFr& grad) {
            Float proj_len = delta_y.cwiseProduct(grad).sum();
            ASSERT_LT(0.0, proj_len);
        }

        Float compute_hausdorff_distance(
                const MatrixFr& vertices_1,
                const MatrixIr& faces_1,
                const MatrixFr& vertices_2) {
            AABBTree tree_1(vertices_1, faces_1);

            VectorF squared_dist;
            VectorI closest_face_indices;

            tree_1.look_up(vertices_2, squared_dist, closest_face_indices);
            return squared_dist.maxCoeff();
        }

        Float area(const Vector3F& v1,
                   const Vector3F& v2,
                   const Vector3F& v3) {
            return ((v2-v1).cross(v3-v1)).norm();
        }

        Vector3F compute_barycentric_coordinate(
                const Vector3F& p,
                const Vector3F& v0,
                const Vector3F& v1,
                const Vector3F& v2) {
            Float a0 = area(p, v1, v2);
            Float a1 = area(p, v2, v0);
            Float a2 = area(p, v0, v1);
            Float total = a0 + a1 + a2;
            return Vector3F(a0, a1, a2) / total;
        }
};

TEST_F(PeriodicExplorationTest, brick5_periodic_timing) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.25);
    explorer.with_parameters(
            m_data_dir + "brick5.orbit",
            m_data_dir + "brick5.modifier");
    explorer.with_refinement("loop", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 - i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                if (dofs[j] < 0) {
                    modified_dofs[j] += i * 0.05;
                } else {
                    modified_dofs[j] -= i * 0.05;
                }
            }
        }
        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate(false);
        write_mesh("test.msh", explorer.get_mesh());
        bool tetgen_success = explorer.run_tetgen();
        ASSERT_TRUE(tetgen_success);

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_brick5_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;;
}

TEST_F(PeriodicExplorationTest, brick5) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.25);
    explorer.with_parameters(
            m_data_dir + "brick5.orbit",
            m_data_dir + "brick5.modifier");
    explorer.with_refinement("loop", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 - i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                if (dofs[j] < 0) {
                    modified_dofs[j] += i * 0.05;
                } else {
                    modified_dofs[j] -= i * 0.05;
                }
            }
        }
        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate(true);
        write_mesh("test.msh", explorer.get_mesh());
        bool tetgen_success = explorer.run_tetgen();
        ASSERT_TRUE(tetgen_success);

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_brick5_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;;
}

TEST_F(PeriodicExplorationTest, pattern0050) {
    PeriodicExploration explorer(
            m_data_dir + "pattern0050.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("loop", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 + i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                modified_dofs[j] -= i * 0.05;
            }
        }
        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate(true);
        write_mesh("debug.msh", explorer.get_mesh());
        bool tetgen_success = explorer.run_tetgen();

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_diamond_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}

TEST_F(PeriodicExplorationTest, finite_difference) {
    std::cout << "This might take a few minutes ";
    std::cout.flush();

    Float eps = 0.1;
    PeriodicExploration explorer(
            m_data_dir + "pattern0050.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("loop", 0);

    size_t num_dofs = explorer.get_num_dofs();
    VectorF dofs = explorer.get_dofs();
    for (size_t i=0; i<num_dofs; i++) {
        if (!explorer.is_thickness_dof(i)) {
            dofs[i] = 0.1;
        }
    }
    explorer.set_dofs(dofs);
    explorer.periodic_inflate(true);

    MatrixFr init_vertices = explorer.get_vertices();
    MatrixIr init_faces = explorer.get_faces();
    auto shape_velocities = explorer.get_shape_velocities();

    for (size_t i=0; i<num_dofs; i++) {
        VectorF modified_dofs = dofs;
        if (explorer.is_thickness_dof(i)) {
            eps = 0.2;
        } else {
            eps = 0.1;
        }
        modified_dofs[i] += eps;

        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate(true);

        MatrixFr vertices = explorer.get_vertices();
        MatrixIr faces = explorer.get_faces();
        MatrixFr perturbed_vertices = init_vertices + shape_velocities[i] * eps;

        std::stringstream sin_1;
        sin_1 << "diamond_perturbation_" << i << ".msh";
        save_mesh(sin_1.str(), perturbed_vertices, init_faces);

        std::stringstream sin_2;
        sin_2 << "diamond_actual_" << i << ".msh";
        save_mesh(sin_2.str(), vertices, faces);

        Float dist = compute_hausdorff_distance(
                perturbed_vertices, init_faces, vertices);
        ASSERT_LT(dist, eps);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}

TEST_F(PeriodicExplorationTest, gradient_descent) {
    std::cout << "This might take a few minutes ";
    std::cout.flush();

    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.25);
    explorer.with_all_isotropic_parameters();
    explorer.with_refinement("simple", 2);

    const size_t num_dofs = explorer.get_num_dofs();
    const size_t num_steps = 5;
    const Float step_size = 0.01;
    for (size_t i=0; i<num_steps; i++) {
        std::stringstream dof_stream;
        dof_stream << "exploration_isotropic_brick5_itr_" << i << ".dof";
        explorer.save_dofs(dof_stream.str());

        explorer.periodic_inflate(true);
        bool tetgen_success = explorer.run_tetgen();
        ASSERT_TRUE(tetgen_success);

        Mesh::Ptr mesh = explorer.get_mesh();

        mesh->add_attribute("vertex_normal");
        VectorF vertex_normals = mesh->get_attribute("vertex_normal");
        ASSERT_EQ(mesh->get_num_vertices() * mesh->get_dim(), vertex_normals.size());

        mesh->add_attribute("vertex_area");
        VectorF vertex_areas = mesh->get_attribute("vertex_area");
        ASSERT_EQ(mesh->get_num_vertices(), vertex_areas.size());

        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        VectorF grad = VectorF::Zero(num_dofs);
        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);

            const size_t num_vertices = mesh->get_num_vertices();
            for (size_t k=0; k<num_vertices; k++) {
                grad[j] += vertex_normals.segment(k*3,3).dot(
                            flattened_attr.segment(k*3,3)) * vertex_areas[k];
            }
        }

        std::stringstream sin;
        sin << "exploration_isotropic_brick5_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        VectorF dofs = explorer.get_dofs();
        dofs += grad * step_size;
        explorer.set_dofs(dofs);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}

TEST_F(PeriodicExplorationTest, debug) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("simple", 2);

    WireNetwork::Ptr wire_network = explorer.get_wire_network();
    MatrixFr vertices = wire_network->get_vertices();

    VectorF dofs(13);
    dofs << 1, 0.872991, 0.784742, 0.424264,
        0.424264, 0.424264, 0.424264,
        0.15, -0.0557327, 0.15,
        0.15, 0.0201534, -0.12166;

    for (size_t i=0; i<3; i++) {
        explorer.set_dofs(dofs);
        try {
            explorer.periodic_inflate(true);
        } catch (...) {
            MatrixFr itr_vertices = wire_network->get_vertices();
            ASSERT_FLOAT_EQ(0.0, (vertices-itr_vertices).norm());
        }
    }
}

TEST_F(PeriodicExplorationTest, DISABLED_debug2) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("simple", 2);
    std::string dof_file = m_data_dir + "lengthError.dof";
    explorer.load_dofs(dof_file);
    try {
        explorer.periodic_inflate(true);
    } catch (const std::exception& e) {
        // TODO: min boundary not match.
    }
}

TEST_F(PeriodicExplorationTest, pattern0746) {
    PeriodicExploration explorer(
            m_data_dir + "pattern0746.wire", 5, 0.5);
    explorer.with_all_isotropic_parameters(ParameterCommon::EDGE);
    ASSERT_EQ(8, explorer.get_num_dofs());
    VectorF dofs(8);
    dofs <<
        0.39285681306678,
        0.3,
        0.4958823475419841,
        0.2691458184615363,
        0.01749434265440056,
        0.1024791517948697,
        0.08633985074680689,
        0.07698883225800063;

    explorer.set_dofs(dofs);
    explorer.periodic_inflate(true);

    MeshPtr mesh = explorer.get_mesh();
    write_mesh("pattern0746.msh", mesh);


    explorer.run_tetgen();

    MatrixFr vertices = explorer.get_vertices();
    ASSERT_LT(0, vertices.rows());
}

TEST_F(PeriodicExplorationTest, printability) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.5);
    explorer.with_all_isotropic_parameters();

    VectorF dofs = explorer.get_dofs();
    ASSERT_EQ(5, dofs.size());

    dofs[4] = -0.1;
    explorer.set_dofs(dofs);
    ASSERT_FALSE(explorer.is_printable());

    dofs[4] = 0.0;
    explorer.set_dofs(dofs);
    ASSERT_TRUE(explorer.is_printable());

    dofs[4] = 0.1;
    explorer.set_dofs(dofs);
    ASSERT_TRUE(explorer.is_printable());
}

TEST_F(PeriodicExplorationTest, hex_profile) {
    PeriodicExploration explorer(
            m_data_dir + "pattern1065.wire", 5, 0.5);
    explorer.with_all_isotropic_parameters(ParameterCommon::EDGE);
    VectorF dofs(6);
    dofs << 0.5, 0.5, 0.5, 0.125, -0.125, 0.125;
    explorer.set_dofs(dofs);
    explorer.with_profile("hexagon");
    explorer.periodic_inflate(true);
    Mesh::Ptr mesh = explorer.get_mesh();
    std::vector<MatrixFr> velocities = explorer.get_shape_velocities();

    std::vector<std::string> attr_names;
    for (size_t j=0; j<6; j++) {
        std::stringstream attr_name_stream;
        attr_name_stream << "velocity_" << j;
        std::string attr_name = attr_name_stream.str();

        VectorF flattened_attr = flatten(velocities[j]);

        mesh->add_attribute(attr_name);
        mesh->set_attribute(attr_name, flattened_attr);
        attr_names.push_back(attr_name);
    }
    save_mesh("hex_1065.msh", mesh, attr_names);
}

TEST_F(PeriodicExplorationTest, shape_velocity) {
    const size_t dim = 3;
    PeriodicExploration explorer(m_data_dir + "pattern1065.wire", 5, 0.5);
    explorer.with_all_isotropic_parameters(ParameterCommon::EDGE);
    VectorF dofs(6);
    dofs << 0.5, 0.5, 0.5, 0.125, -0.125, 0.125;
    explorer.set_dofs(dofs);

    explorer.periodic_inflate(true);
    MatrixFr reflected_vertices = explorer.get_vertices();
    MatrixIr reflected_faces = explorer.get_faces();
    auto reflected_shape_velocities = explorer.get_shape_velocities();

    explorer.periodic_inflate(false);
    MatrixFr normal_vertices = explorer.get_vertices();
    MatrixIr normal_faces = explorer.get_faces();
    auto normal_shape_velocities = explorer.get_shape_velocities();
    auto normal_face_sources = explorer.get_face_sources();
    const size_t num_normal_faces = normal_faces.rows();
    std::vector<VectorI> faces_with_valid_source;
    for (size_t i=0; i<num_normal_faces; i++) {
        if (normal_face_sources[i] != 0) {
            faces_with_valid_source.push_back(normal_faces.row(i));
        }
    }
    MatrixIr non_periodic_normal_faces = MatrixUtils::rowstack(faces_with_valid_source);

    ASSERT_EQ(reflected_shape_velocities.size(), normal_shape_velocities.size());

    AABBTree tree(normal_vertices, non_periodic_normal_faces);

    VectorF sq_dist;
    VectorI face_indices;
    MatrixFr closest_points;
    tree.look_up_with_closest_points(reflected_vertices,
            sq_dist, face_indices, closest_points);

    const size_t num_velocities = normal_shape_velocities.size();
    const size_t num_reflected_vertices = reflected_vertices.rows();
    ASSERT_EQ(num_reflected_vertices, face_indices.size());
    ASSERT_EQ(num_reflected_vertices, closest_points.rows());

    std::vector<MatrixFr> interpolated_velocities;
    std::vector<VectorF> velocity_diff;
    for (size_t i=0; i<num_velocities; i++) {
        interpolated_velocities.push_back(
                MatrixFr::Zero(num_reflected_vertices, dim));
        velocity_diff.push_back(VectorF::Zero(num_reflected_vertices));
    }
    for (size_t i=0; i<num_reflected_vertices; i++) {
        const size_t fid = face_indices[i];
        const VectorF& p = closest_points.row(i);
        const Float dist = sq_dist[i];
        if (dist > 1e-6) continue;

        const VectorI& f = normal_faces.row(fid);
        Vector3F barycentric_coord = compute_barycentric_coordinate(
                p,
                normal_vertices.row(f[0]),
                normal_vertices.row(f[1]),
                normal_vertices.row(f[2]));
        ASSERT_TRUE(barycentric_coord.allFinite());
        for (size_t j=0; j<num_velocities; j++) {
            const VectorF& reflected_velocity =
                reflected_shape_velocities[j].row(i);
            VectorF interpolated_velocity =
                normal_shape_velocities[j].row(f[0]) * barycentric_coord[0] +
                normal_shape_velocities[j].row(f[1]) * barycentric_coord[1] +
                normal_shape_velocities[j].row(f[2]) * barycentric_coord[2];
            interpolated_velocities[j].row(i) = interpolated_velocity;
            //std::cout << reflected_velocity.transpose() << std::endl;
            //std::cout << interpolated_velocity.transpose() << std::endl;
            //ASSERT_NEAR(0.0, (reflected_velocity - interpolated_velocity).norm(), 1e-3);
            velocity_diff[j][i] = (reflected_velocity - interpolated_velocity).norm();
        }
    }

    Mesh::Ptr reflected_mesh = form_mesh(reflected_vertices, reflected_faces);
    Mesh::Ptr normal_mesh = form_mesh(normal_vertices, normal_faces);
    for (size_t i=0; i<num_velocities; i++) {
        std::stringstream attr_name;
        attr_name << "velocity_" << i;
        VectorF reflected_velocity =
                MatrixUtils::flatten<VectorF>(
                    reflected_shape_velocities[i]);
        reflected_mesh->add_attribute(attr_name.str());
        reflected_mesh->set_attribute(attr_name.str(), reflected_velocity);

        VectorF interpolated_velocity =
                MatrixUtils::flatten<VectorF>(
                    interpolated_velocities[i]);
        reflected_mesh->add_attribute(attr_name.str()+"_interpolated");
        reflected_mesh->set_attribute(attr_name.str()+"_interpolated",
                interpolated_velocity);

        std::stringstream diff_name;
        diff_name << "velocity_diff_" << i;
        reflected_mesh->add_attribute(diff_name.str());
        reflected_mesh->set_attribute(diff_name.str(), velocity_diff[i]);

        VectorF normal_velocity = MatrixUtils::flatten<VectorF>(
                normal_shape_velocities[i]);
        normal_mesh->add_attribute(attr_name.str());
        normal_mesh->set_attribute(attr_name.str(), normal_velocity);
    }
    save_mesh("shape_velocity_compare_reflected.msh", reflected_mesh,
            reflected_mesh->get_attribute_names());
    save_mesh("shape_velocity_compare_normal.msh", normal_mesh,
            normal_mesh->get_attribute_names());
}
