#include "MeshFactory.h"
#include <cassert>
#include <tr1/memory>

#include "Exception.h"
#include "Mesh.h"
#include "MeshAttributes.h"
#include "MeshConnectivity.h"
#include "MeshGeometry.h"
#include "MeshParser.h"

MeshFactory::MeshFactory() {
    m_mesh = new Mesh();
}

MeshFactory& MeshFactory::load_file(const std::string& filename) {
    MeshParser* parser = MeshParser::create_parser(filename);
    assert(parser != NULL);
    bool success = parser->parse(filename);
    assert(success);

    m_mesh->set_geometry(Mesh::GeometryPtr(new MeshGeometry()));
    initialize_vertices(parser);
    initialize_faces(parser);
    initialize_voxels(parser);
    initialize_attributes(parser);

    delete parser;
    return *this;
}

MeshFactory& MeshFactory::load_data(VectorF& vertices, VectorI& faces, VectorI& voxels,
        size_t num_vertex_per_face, size_t num_vertex_per_voxel) {
    m_mesh->set_geometry(Mesh::GeometryPtr(new MeshGeometry()));
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();
    geometry->set_vertices(vertices);
    geometry->set_faces(faces);
    geometry->set_voxels(voxels);
    geometry->set_vertex_per_face(num_vertex_per_face);
    geometry->set_vertex_per_voxel(num_vertex_per_voxel);
    return *this;
}

MeshFactory& MeshFactory::with_connectivity(std::string& conn_type) {
    // Valid conn_type are: vertex, face, voxel, all
    // Using minimal prefix to distinguish them.
    if (conn_type.size() < 2) {
        conn_type = std::string("all");
    }

    if (conn_type[0] == 'v' && conn_type[1] == 'e') {
        m_mesh->enable_vertex_connectivity();
    } else if (conn_type[0] == 'v' && conn_type[1] == 'o') {
        m_mesh->enable_voxel_connectivity();
    } else if (conn_type[0] == 'e') {
        m_mesh->enable_face_connectivity();
    } else if (conn_type[0] == 'a') {
        m_mesh->enable_connectivity();
    } else {
        std::string err = "unknown connectivity type ";
        err += conn_type;
        throw RuntimeError(err);
    }

    return *this;
}

MeshFactory& MeshFactory::with_attribute(const std::string& attr_name) {
    Mesh::AttributesPtr attributes = m_mesh->get_attributes();
    attributes->add_attribute(attr_name, *m_mesh);
    return *this;
}

void MeshFactory::initialize_vertices(MeshParser* parser) {
    Mesh::GeometryPtr geometry = m_mesh->get_geometry();

    VectorF& vertices = geometry->get_vertices();
    vertices.resize(parser->num_vertices() * 3);

    parser->export_vertices(vertices.data());
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
