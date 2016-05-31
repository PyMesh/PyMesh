#include "MeshFactory.h"
#include <cassert>
#include <iostream>
#include <sstream>

#include <Attributes/MeshAttributes.h>
#include <Connectivity/MeshConnectivity.h>
#include <Core/Exception.h>
#include <Geometry/MeshGeometry.h>
#include <IO/MeshParser.h>
#include <Mesh.h>

using namespace PyMesh;

MeshFactory::MeshFactory() {
    m_mesh = new Mesh();
}

MeshFactory& MeshFactory::load_file(const std::string& filename) {
    MeshParser* parser = MeshParser::create_parser(filename);
    assert(parser != NULL);
    bool success = parser->parse(filename);
    if (!success) {
        std::stringstream err_msg;
        err_msg << "Parsing " << filename << " has failed.";
        throw RuntimeError(err_msg.str());
    }

    m_mesh->set_geometry(Mesh::GeometryPtr(new MeshGeometry()));
    initialize_vertices(parser);
    initialize_faces(parser);
    initialize_voxels(parser);
    initialize_attributes(parser);

    delete parser;
    return *this;
}

MeshFactory& MeshFactory::load_data(
        const VectorF& vertices, const VectorI& faces, const VectorI& voxels,
        size_t dim, size_t num_vertex_per_face, size_t num_vertex_per_voxel) {
    m_mesh->set_geometry(Mesh::GeometryPtr(new MeshGeometry()));
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();
    geometry->set_vertices(vertices);
    geometry->set_faces(faces);
    geometry->set_voxels(voxels);
    geometry->set_dim(dim);
    geometry->set_vertex_per_face(num_vertex_per_face);
    geometry->set_vertex_per_voxel(num_vertex_per_voxel);

    if (faces.size() == 0 && voxels.size() > 0) {
        geometry->extract_faces_from_voxels();
    }

    return *this;
}

MeshFactory& MeshFactory::with_connectivity(
        const std::string& conn_type) {
    // Valid conn_type are: vertex, face, voxel, all
    // Using minimal prefix to distinguish them.
    const size_t l = conn_type.size();
    if (l >= 2 && conn_type[0] == 'v' && conn_type[1] == 'e') {
        m_mesh->enable_vertex_connectivity();
    } else if (l >= 2 && conn_type[0] == 'v' && conn_type[1] == 'o') {
        m_mesh->enable_voxel_connectivity();
    } else if (l >= 1 && conn_type[0] == 'f') {
        m_mesh->enable_face_connectivity();
    } else if (l >= 1 && conn_type[0] == 'a') {
        m_mesh->enable_connectivity();
    } else {
        std::string err = "unknown connectivity type (";
        err += conn_type;
        err += ")";
        throw RuntimeError(err);
    }

    return *this;
}

MeshFactory& MeshFactory::with_attribute(
        const std::string& attr_name) {
    Mesh::AttributesPtr attributes = m_mesh->get_attributes();
    attributes->add_attribute(attr_name, *m_mesh);
    return *this;
}

MeshFactory& MeshFactory::drop_zero_dim() {
    compute_and_drop_zero_dim();
    return *this;
}

void MeshFactory::initialize_vertices(MeshParser* parser) {
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();

    VectorF& vertices = geometry->get_vertices();
    vertices.resize(parser->num_vertices() * parser->dim());

    parser->export_vertices(vertices.data());
    geometry->set_dim(parser->dim());
}

void MeshFactory::initialize_faces(MeshParser* parser) {
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();

    VectorI& faces = geometry->get_faces();
    faces.resize(parser->num_faces() * parser->vertex_per_face());

    parser->export_faces(faces.data());
    geometry->set_vertex_per_face(parser->vertex_per_face());
}

void MeshFactory::initialize_voxels(MeshParser* parser) {
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();

    VectorI& voxels = geometry->get_voxels();
    voxels.resize(parser->num_voxels() * parser->vertex_per_voxel());

    parser->export_voxels(voxels.data());
    geometry->set_vertex_per_voxel(parser->vertex_per_voxel());
}

void MeshFactory::initialize_attributes(MeshParser* parser) {
    Mesh::AttributesPtr attributes = m_mesh->get_attributes();

    MeshParser::AttrNames attr_names = parser->get_attribute_names();
    for (MeshParser::AttrNames::const_iterator itr = attr_names.begin();
            itr != attr_names.end(); itr++) {
        const std::string& name = *itr;
        size_t attr_size = parser->get_attribute_size(name);
        VectorF attr_data(attr_size);
        parser->export_attribute(name, attr_data.data());

        attributes->add_empty_attribute(name);
        attributes->set_attribute(name, attr_data);
    }
}

void MeshFactory::compute_and_drop_zero_dim() {
    const size_t num_vertices = m_mesh->get_num_vertices();
    if (num_vertices == 0) return;
    int zero_dim = m_mesh->get_geometry()->project_out_zero_dim();
    if (zero_dim < 0) return;
    const char axis[3] = {'X', 'Y', 'Z'};
    std::cerr << "Dropping " << axis[zero_dim]
        << " coordinate because flat geometry." << std::endl;
    std::vector<std::string> attr_names = m_mesh->get_attribute_names();
    for (auto itr = attr_names.begin(); itr != attr_names.end(); itr++) {
        VectorF& attr = m_mesh->get_attribute(*itr);
        if (attr.size() == num_vertices * 3) {
            VectorF reduced_attr(num_vertices * 2);
            for (size_t i=0; i<num_vertices; i++) {
                reduced_attr[i*2  ] = (zero_dim == 0) ?
                    attr[i*3+1] : attr[i*3];
                reduced_attr[i*2+1] = (zero_dim == 2) ?
                    attr[i*3+1] : attr[i*3+2];
            }
            m_mesh->set_attribute(*itr, reduced_attr);
        }
    }
}

