#include "OBJParser.h"
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <list>

#include "EigenTypedef.h"

bool OBJParser::parse(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "Error: failed to open file \"" << filename << "\""
            << std::endl;
        return false;
    }

    bool success;
    while (!fin.eof()) {
        fin.getline(line, LINE_SIZE);
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
    return true;
}

size_t OBJParser::num_attributes() const {
    if (has_normals()) return 1;
    else return 0;
}

OBJParser::AttrNames OBJParser::get_attribute_names() const {
    OBJParser::AttrNames attr_names;
    if (has_normals())
        attr_names.push_back("vertex_normal");
    return attr_names;
}

size_t OBJParser::get_attribute_size(const std::string& name) const {
    if (!attribute_exists(name)) {
        std::cerr << "Attribute " << name << " does not exist." << std::endl;
        return 0;
    }
    return m_vertex_normals.size() * 3;
}

void OBJParser::export_vertices(Float* buffer) {
    size_t count=0;
    for (VertexList::const_iterator vi=m_vertices.begin();
            vi != m_vertices.end(); vi++) {
        const Vector3F& v = *vi;
        buffer[count*3  ] = v[0];
        buffer[count*3+1] = v[1];
        buffer[count*3+2] = v[2];
        count++;
    }
}

void OBJParser::export_faces(int* buffer) {
    size_t count=0;
    for (FaceList::const_iterator fi=m_faces.begin();
            fi != m_faces.end(); fi++) {
        const Vector3I& f = *fi;
        buffer[count*3  ] = f[0];
        buffer[count*3+1] = f[1];
        buffer[count*3+2] = f[2];
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
    if (!attribute_exists(name)) {
        std::cerr << "Warning: mesh does not have attribute with name "
            << name << std::endl;
        return;
    }

    size_t count=0;
    for (NormalList::const_iterator ni = m_vertex_normals.begin();
            ni != m_vertex_normals.end(); ni++) {
        const Vector3F& normal = *ni;
        buffer[count*3  ] = normal[0];
        buffer[count*3+1] = normal[1];
        buffer[count*3+2] = normal[2];
        count++;
    }
}


bool OBJParser::has_normals() const {
    return !(m_vertex_normals.size() == 0);
}

bool OBJParser::attribute_exists(const std::string& name) const {
    // Only normal attributes are supported.
    return (name == "vertex_normal");
}

bool OBJParser::parse_vertex_line(char* line) {
    char header[8];
    Float x,y,z,w;
    size_t n = sscanf(line, "%s %lf %lf %lf %lf", header, &x, &y, &z, &w);
    if (n < 4) return false;

    // Check to handle homogeneous coordinates.
    if (n == 5) {
        x = x/w;
        y = y/w;
        z = z/w;
    }

    if (header[1] == '\0')
        m_vertices.push_back(Vector3F(x,y,z));
    else if (header[1] == 'n')
        m_vertex_normals.push_back(Vector3F(x,y,z));
    return true;
}

bool OBJParser::parse_face_line(char* line) {
    const char WHITE_SPACE[] = " \t\n";
    char* field = strtok(line, WHITE_SPACE);
    assert(field != NULL);

    // Ignore header "f"
    field = strtok(NULL, WHITE_SPACE);

    // Extract vertex idx
    size_t idx[4];
    int v_idx, vt_idx, vn_idx;
    size_t n;
    size_t i=0;
    for (i=0; i<4 && field != NULL; i++) {
        // Note each vertex field could be in any of the following formats:
        // v_idx  or  v_idx/vt_idx  or  v_idx/vt_idx/vn_idx
        // Only v_idx is extracted for now.
        n = sscanf(field, "%i/%i/%i", &v_idx, &vt_idx, &vn_idx);
        if (n < 1) {
            return false;
        }
        assert(v_idx > 0);
        idx[i] = v_idx-1; // OBJ has index starting from 1

        // Get next token
        field = strtok(NULL, WHITE_SPACE);
    }
    if (field != NULL) {
        std::cerr << "Only triangle and quad meshes are supported" << std::endl;
        return false;
    }

    m_faces.push_back(Vector3I(idx[0], idx[1], idx[2]));
    if (i == 4) {
        // break quad into triangles too.
        m_faces.push_back(Vector3I(idx[0], idx[2], idx[3]));
    }
    return true;
}
