/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OBJParser.h"
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

OBJParser::OBJParser() :
    m_dim(0),
    m_vertex_per_face(0),
    m_texture_dim(0),
    m_parameter_dim(0){ }

bool OBJParser::parse(const std::string& filename) {
    const size_t LINE_SIZE = 4096;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::stringstream err_msg;
        err_msg << "failed to open file \"" << filename << "\"";
        throw IOError(err_msg.str());
    }

    bool success;
    while (!fin.eof()) {
        fin.getline(line, LINE_SIZE);
        char* continue_char = strrchr(line, '\\');
        while (continue_char != NULL &&
                (continue_char - line + 1) == strlen(line)) {
            fin.getline(continue_char, LINE_SIZE-(continue_char - line + 1));
            continue_char = strchr(line, '\\');
        }

        switch (line[0]) {
            case 'v':
                success = parse_vertex_line(line);
                break;
            case 'f':
                success = parse_face_line(line);
                break;
            default:
                // Ignore other lines by default.
                success = true;
                break;
        }
        if (!success) return false;
    }

    fin.close();
    unify_faces();

    if (m_vertices.size() == 0) {
        m_dim = 3; // default: 3D
    }
    if (m_faces.size() == 0) {
        m_vertex_per_face = 3; // default: triangle
    }
    finalize_textures();
    finalize_normals();
    finalize_parameters();
    return true;
}

size_t OBJParser::num_attributes() const {
    size_t r = 0;
    if (m_corner_normals.size() > 0) r++;
    if (m_corner_textures.size() > 0) r++;
    if (m_parameters.size() > 0) r++;
    return r;
}

OBJParser::AttrNames OBJParser::get_attribute_names() const {
    OBJParser::AttrNames attr_names;
    if (m_corner_normals.size() > 0)
        attr_names.push_back("corner_normal");
    if (m_corner_textures.size() > 0)
        attr_names.push_back("corner_texture");
    if (m_parameters.size() > 0)
        attr_names.push_back("vertex_parameter");
    return attr_names;
}

size_t OBJParser::get_attribute_size(const std::string& name) const {
    if (name == "corner_normal")
        return m_corner_normals.size() * m_dim;
    else if (name == "corner_texture")
        return m_corner_textures.size() * m_texture_dim;
    else if (name == "vertex_parameter")
        return m_parameters.size() * m_parameter_dim;
    else {
        std::cerr << "Attribute " << name << " does not exist." << std::endl;
        return 0;
    }
}

void OBJParser::export_vertices(Float* buffer) {
    const size_t dim = m_dim;
    size_t count=0;
    for (VertexList::const_iterator vi=m_vertices.begin();
            vi != m_vertices.end(); vi++) {
        const VectorF& v = *vi;
        for (size_t i=0; i<dim; i++) {
            buffer[dim * count + i] = v[i];
        }
        count++;
    }
}

void OBJParser::export_faces(int* buffer) {
    size_t count=0;
    for (FaceList::const_iterator fi=m_faces.begin();
            fi != m_faces.end(); fi++) {
        const VectorI& f = *fi;
        assert(f.size() == m_vertex_per_face);
        std::copy(f.data(), f.data() + m_vertex_per_face,
                buffer + count*m_vertex_per_face);
        count++;
    }
}

void OBJParser::export_voxels(int* buffer) {
    size_t num_vertex_per_voxel = vertex_per_voxel();
    size_t count=0;
    for (VoxelList::const_iterator vi=m_voxels.begin();
            vi != m_voxels.end(); vi++) {
        const VectorI& v = *vi;
        const int* voxel = v.data();
        std::copy(voxel, voxel + num_vertex_per_voxel,
                buffer + num_vertex_per_voxel * count);
        count++;
    }
}

void OBJParser::export_attribute(const std::string& name, Float* buffer) {
    if (name == "corner_normal")
        export_normals(buffer);
    else if (name == "corner_texture")
        export_textures(buffer);
    else if (name == "vertex_parameter")
        export_parameters(buffer);
    else {
        std::cerr << "Warning: mesh does not have attribute with name "
            << name << std::endl;
    }
}

void OBJParser::export_normals(Float* buffer) const {
    const size_t dim = m_dim;
    size_t count=0;
    for (const auto& n : m_corner_normals) {
        std::copy(n.data(), n.data() + dim, buffer+count*dim);
        count++;
    }
}

void OBJParser::export_textures(Float* buffer) const {
    const size_t dim = m_texture_dim;
    size_t count=0;
    for (const auto& t : m_corner_textures) {
        std::copy(t.data(), t.data() + dim, buffer+count*dim);
        count++;
    }
}

void OBJParser::export_parameters(Float* buffer) const {
    const size_t dim = m_parameter_dim;
    size_t count=0;
    for (const auto& p : m_parameters) {
        std::copy(p.data(), p.data() + dim, buffer+count*dim);
        count++;
    }
}

bool OBJParser::parse_vertex_line(char* line) {
    assert(line[0] == 'v');
    switch (line[1]) {
        case ' ':
        case '\t':
            return parse_vertex_coordinate(line);
        case 't':
            return parse_vertex_texture(line);
        case 'n':
            return parse_vertex_normal(line);
        case 'p':
            return parse_vertex_parameter(line);
        default:
            throw IOError("Invalid vertex line");
    }
}

bool OBJParser::parse_vertex_coordinate(char* line) {
    char header[8];
    Float data[4];
    size_t n = sscanf(line, "%s %lf %lf %lf %lf", header,
            &data[0], &data[1], &data[2], &data[3]);
    if (n < 3) return false;
    assert(strcmp(header, "v") == 0);

    // Check to handle homogeneous coordinates.
    if (n == 5) {
        data[0] /= data[3];
        data[1] /= data[3];
        data[2] /= data[3];
        n -= 1;
    }
    if (m_dim == 0) { m_dim = n-1; }
    else if (m_dim != n-1) { return false; }

    Eigen::Map<VectorF> coord(data, m_dim);
    m_vertices.push_back(coord);
    return true;
}

bool OBJParser::parse_vertex_normal(char* line) {
    char header[8];
    Float data[3];
    size_t n = sscanf(line, "%s %lf %lf %lf", header,
            &data[0], &data[1], &data[2]);
    if (n < 3) return false;
    assert(strcmp(header, "vn") == 0);
    m_corner_normals.emplace_back(Eigen::Map<VectorF>(data, n-1));
    return true;
}

bool OBJParser::parse_vertex_texture(char* line) {
    char header[8];
    Float data[3];
    size_t n = sscanf(line, "%s %lf %lf %lf", header,
            &data[0], &data[1], &data[2]);
    if (n < 3) return false;
    assert(strcmp(header, "vt") == 0);
    m_corner_textures.emplace_back(Eigen::Map<VectorF>(data, n-1));
    return true;
}

bool OBJParser::parse_vertex_parameter(char* line) {
    char header[8];
    Float data[3];
    size_t n = sscanf(line, "%s %lf %lf %lf", header,
            &data[0], &data[1], &data[2]);
    if (n < 3) return false;
    assert(strcmp(header, "vp") == 0);
    m_parameters.emplace_back(Eigen::Map<VectorF>(data, n-1));
    return true;
}

bool OBJParser::parse_face_line(char* line) {
    const char WHITE_SPACE[] = " \t\n\r";
    char* field = strtok(line, WHITE_SPACE);
    assert(field != NULL);

    // Ignore header "f"
    field = strtok(NULL, WHITE_SPACE);

    // Extract vertex idx
    std::vector<size_t> idx;
    std::vector<size_t> t_idx;
    std::vector<size_t> n_idx;
    int v_idx, vt_idx, vn_idx;
    size_t n;
    size_t i=0;
    while (field != NULL) {
        // Note each vertex field could be in any of the following formats:
        // v_idx  or  v_idx/vt_idx  or  v_idx/vt_idx/vn_idx or v_idx//vn_idx
        v_idx = vt_idx = vn_idx = 0;
        v_idx = atoi(field);
        char* loc = strchr(field, '/');
        if (loc != NULL) {
            loc++;
            vt_idx = atoi(loc);
            loc = strchr(loc, '/');
            if (loc != NULL) {
                loc++;
                vn_idx = atoi(loc);
            }
        }

        if (v_idx == 0) return false;

        // Negative index means relative index from the vertices read so
        // far.  -1 refers to the last vertex read in.
        if (v_idx < 0) {
            v_idx = m_vertices.size() + v_idx + 1;
        }
        if (vt_idx < 0) {
            vt_idx = m_corner_textures.size() + vt_idx + 1;
        }
        if (vn_idx < 0) {
            vn_idx = m_corner_normals.size() + vn_idx + 1;
        }
        assert(v_idx > 0);
        assert(vt_idx >= 0);
        assert(vn_idx >= 0);
        idx.push_back(v_idx-1); // OBJ has index starting from 1
        if (vt_idx != 0) t_idx.push_back(vt_idx-1);
        if (vn_idx != 0) n_idx.push_back(vn_idx-1);

        // Get next token
        field = strtok(NULL, WHITE_SPACE);
    }

    const size_t num_idx_parsed = idx.size();
    if (num_idx_parsed == 3) {
        m_tris.push_back(Vector3I(idx[0], idx[1], idx[2]));
        if (!t_idx.empty())
            m_tri_textures.push_back(Vector3I(t_idx[0], t_idx[1], t_idx[2]));
        if (!n_idx.empty())
            m_tri_normals.push_back(Vector3I(n_idx[0], n_idx[1], n_idx[2]));
    } else if (num_idx_parsed == 4) {
        m_quads.push_back(Vector4I(idx[0], idx[1], idx[2], idx[3]));
        if (!t_idx.empty())
            m_quad_textures.push_back(
                    Vector4I(t_idx[0], t_idx[1], t_idx[2], t_idx[3]));
        if (!n_idx.empty())
            m_quad_normals.push_back(
                    Vector4I(n_idx[0], n_idx[1], n_idx[2], n_idx[3]));
    } else {
        // N-gon detected, assuming it is convex and break it into triangles.
        std::cerr << num_idx_parsed << "-gon detected, converting to triangles"
            << std::endl;
        for (size_t i=1; i<num_idx_parsed-1; i++) {
            m_tris.push_back(Vector3I(idx[0], idx[i], idx[i+1]));
        }
    }
    return true;
}

void OBJParser::unify_faces() {
    if (m_tris.size() > 0 && m_quads.size() == 0) {
        m_faces = std::move(m_tris);
        m_textures = std::move(m_tri_textures);
        m_normals = std::move(m_tri_normals);
        m_vertex_per_face = 3;
    } else if (m_tris.size() == 0 && m_quads.size() > 0) {
        m_faces = std::move(m_quads);
        m_textures = std::move(m_quad_textures);
        m_normals = std::move(m_quad_normals);
        m_vertex_per_face = 4;
    } else if (m_tris.size() > 0 && m_quads.size() > 0){
        std::cerr << "Mixed triangle and quads in the input file" << std::endl;
        std::cerr << "Converting quads in triangles, face order is not kept!"
            << std::endl;
        m_faces = std::move(m_tris);
        for (auto& quad : m_quads) {
            m_faces.push_back(Vector3I(quad[0], quad[1], quad[2]));
            m_faces.push_back(Vector3I(quad[0], quad[2], quad[3]));
        }
        m_textures = std::move(m_tri_textures);
        for (auto& tex : m_quad_textures) {
            m_textures.push_back(Vector3I(tex[0], tex[1], tex[2]));
            m_textures.push_back(Vector3I(tex[0], tex[2], tex[3]));
        }
        m_normals = std::move(m_tri_normals);
        for (auto& n : m_quad_normals) {
            m_normals.push_back(Vector3I(n[0], n[1], n[2]));
            m_normals.push_back(Vector3I(n[0], n[2], n[3]));
        }
        m_vertex_per_face = 3;
    }
}

void OBJParser::finalize_textures() {
    const size_t num_faces = m_faces.size();
    if (m_textures.size() != num_faces || m_corner_textures.size() == 0)
        return;

    m_texture_dim = 2;
    bool bad_texture = false;
    const size_t num_corner_textures = m_corner_textures.size();
    for (const auto& t : m_textures) {
        if (t.size() != m_vertex_per_face) {
            std::cerr << "Texture and face type mismatch." << std::endl;
            bad_texture = true;
            break;
        }
        if (t.minCoeff() < 0 || t.maxCoeff() >= num_corner_textures) {
            std::cerr << "Texture index out of bound." << std::endl;
            bad_texture = true;
            break;
        }
        //if (t.size() != m_texture_dim) {
        //    std::cerr << t << std::endl;
        //    std::cerr << "Texture has the wrong dimension." << std::endl;
        //    bad_texture = true;
        //    break;
        //}
    }

    if (bad_texture) {
        m_textures.clear();
        return;
    }

    TextureVector textures;
    for (const auto& t : m_textures) {
        for (size_t i=0; i<m_vertex_per_face; i++) {
            textures.emplace_back(m_corner_textures[t[i]]);
        }
    }
    std::swap(textures, m_corner_textures);
}

void OBJParser::finalize_normals() {
    if (m_normals.size() != m_faces.size() || m_corner_normals.size() == 0)
        return;

    bool bad_normal = false;
    const size_t num_corner_normals = m_corner_normals.size();
    for (const auto& n : m_normals) {
        if (n.size() != m_vertex_per_face) {
            std::cerr << "Normal and face type mismatch." << std::endl;
            bad_normal = true;
            break;
        }
        if (n.minCoeff() < 0 || n.maxCoeff() >= num_corner_normals) {
            std::cerr << "Normalindex out of bound: <" << n.transpose()
                << "> exceeds " << num_corner_normals << "."
                << std::endl;
            bad_normal = true;
            break;
        }
    }

    if (bad_normal) {
        m_normals.clear();
        return;
    }

    NormalVector normals;
    for (const auto& n : m_normals) {
        for (size_t i=0; i<m_vertex_per_face; i++) {
            normals.emplace_back(m_corner_normals[n[i]]);
        }
    }
    std::swap(normals, m_corner_normals);
}

void OBJParser::finalize_parameters() {
    if (m_parameters.empty()) return;
    if (m_parameters.size() != m_vertices.size()) {
        std::cerr << "Mismatch between vertex and vertex parameters."
            << std::endl;
        m_parameters.clear();
    }
    m_parameter_dim = m_parameters.front().size();
    for (const auto& n : m_parameters) {
        if (n.size() != m_parameter_dim) {
            std::cerr << "Inconsistent parameter dimension" << std::endl;
            m_parameter_dim = 0;
            break;
        }
    }
    if (m_parameter_dim == 0) {
        m_parameters.clear();
    }
}
