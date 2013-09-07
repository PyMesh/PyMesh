#include "STLParser.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>

#include "IOUtils.h"
#include "HashGrid.h"

using namespace Zhou;

bool STLParser::parse(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "Error: failed to open file \"" << filename << "\""
            << std::endl;
        return false;
    }

    fin.getline(line, LINE_SIZE);
    bool is_binary = parse_header(line);
    fin.close();

    bool success = false;
    if (is_binary) {
        success = parse_binary(filename);
    } else {
        success = parse_ascii(filename);
    }
    assert(success);
    if (!success) return false;

    validate_normals();
    merge_identical_vertices();
    return true;
}

size_t STLParser::num_attributes() const {
    if (has_normal())
        return 1;
    else
        return 0;
}

STLParser::AttrNames STLParser::get_attribute_names() const {
    AttrNames names;
    if (has_normal())
        names.push_back("face_normal");
    return names;
}

size_t STLParser::get_attribute_size(const std::string& name) const {
    if (!attribute_exists(name)) {
        std::cerr << "Warning: mesh does not have attribute with name "
            << name << std::endl;
        return 0;
    }

    if (has_normal())
        return m_facet_normals.size() * 3;
    else
        return 0;
}

void STLParser::export_vertices(Float* buffer) {
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

void STLParser::export_faces(int* buffer) {
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

void STLParser::export_voxels(int* buffer) {
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

void STLParser::export_attribute(const std::string& name, Float* buffer) {
    if (!attribute_exists(name)) {
        std::cerr << "Warning: mesh does not have attribute with name "
            << name << std::endl;
        return;
    }

    size_t count=0;
    for (NormalList::const_iterator ni = m_facet_normals.begin();
            ni != m_facet_normals.end(); ni++) {
        const Vector3F& normal = *ni;
        buffer[count*3  ] = normal[0];
        buffer[count*3+1] = normal[1];
        buffer[count*3+2] = normal[2];
        count++;
    }
}

bool STLParser::attribute_exists(const std::string& name) const {
    return has_normal() && (name == "face_normal");
}

bool STLParser::parse_header(char* line) {
    return !is_prefix("solid", line);
}

bool STLParser::parse_ascii(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];
    bool success = true;

    std::ifstream fin(filename.c_str());

    // skip header line.
    fin.getline(line, LINE_SIZE);

    while (!fin.eof()) {
        success = parse_ascii_facet(fin);
        if (!success) return false;
    }

    fin.close();
}

bool STLParser::parse_ascii_facet(std::ifstream& fin) {
    const size_t LINE_SIZE = 256;
    const size_t WORD_SIZE = 128;
    char line[LINE_SIZE];
    char first_word[WORD_SIZE];
    const std::string face_begin("facet");
    const std::string face_end("endfacet");
    const std::string loop_begin("outer");
    const std::string loop_end("endloop");
    const std::string vertex_flag("vertex");

    bool reading_facet = false;
    bool reading_loop = false;
    bool success = true;
    size_t num_vts = 0;
    while (!fin.eof()) {
        fin.getline(line, LINE_SIZE);
        size_t n = sscanf(line, " %s", first_word);
        if (n == 0) continue;
        if (is_prefix(face_begin.c_str(), first_word)) {
            success = parse_ascii_normal(line);
            assert(success);
            reading_facet = true;
        } else if (is_prefix(face_end.c_str(), first_word)) {
            assert(reading_facet);
            reading_facet = false;
        } else if (is_prefix(loop_begin.c_str(), first_word)) {
            reading_loop = true;
        } else if (is_prefix(loop_end.c_str(), first_word)) {
            assert(reading_loop);
            reading_loop = false;
        } else if (is_prefix(vertex_flag.c_str(), first_word)) {
            assert(reading_facet);
            assert(reading_loop);
            success = parse_ascii_vertex(line);
            assert(success);
            num_vts += 1;
        }
        if (!success) return false;
        if (!reading_facet) break;
    }
    if (num_vts == 0) return true;
    assert(num_vts == 3);
    if (num_vts != 3) {
        std::cerr << "Warning: mesh contain face made of "
            << num_vts << " vertices" << std::endl;
        return false;
    }
    VectorI f(num_vts);
    size_t start_idx = m_vertices.size() - num_vts;
    for (size_t i=0; i<num_vts; i++) {
        f[i] = start_idx + i;
    }
    m_faces.push_back(f);
    return true;
}

bool STLParser::parse_ascii_normal(char* line) {
    Float x,y,z;
    size_t n = sscanf(line, " facet normal %lf %lf %lf", &x, &y, &z);
    assert(n == 3);
    if (n != 3) return false;

    Vector3F normal(x,y,z);
    m_facet_normals.push_back(normal);
    return true;
}

bool STLParser::parse_ascii_vertex(char* line) {
    Float x,y,z;
    size_t n = sscanf(line, " vertex %lf %lf %lf", &x, &y, &z);
    assert(n == 3);
    if (n != 3) return false;

    Vector3F v(x,y,z);
    m_vertices.push_back(v);
    return true;
}

bool STLParser::parse_binary(const std::string& filename) {
    std::ifstream fin(filename.c_str());

    size_t float_size = sizeof(float);
    assert(float_size == 4);
    size_t LINE_SIZE=256;
    char buf[LINE_SIZE];

    // 80 bytes header, no data significance.
    fin.read(buf, 80);

    fin.read(buf, 4);
    size_t num_faces = *reinterpret_cast<unsigned int*>(buf);

    for (size_t i=0; i<num_faces; i++) {
        // Parse normal
        fin.read(buf, float_size*3);
        float nx = *reinterpret_cast<float*>(buf);
        float ny = *reinterpret_cast<float*>(buf + float_size);
        float nz = *reinterpret_cast<float*>(buf + float_size * 2);
        assert(fin.good());

        // vertex 1
        fin.read(buf, float_size*3);
        float v1x = *reinterpret_cast<float*>(buf);
        float v1y = *reinterpret_cast<float*>(buf + float_size);
        float v1z = *reinterpret_cast<float*>(buf + float_size * 2);
        assert(fin.good());

        // vertex 2
        fin.read(buf, float_size*3);
        float v2x = *reinterpret_cast<float*>(buf);
        float v2y = *reinterpret_cast<float*>(buf + float_size);
        float v2z = *reinterpret_cast<float*>(buf + float_size * 2);
        assert(fin.good());

        // vertex 3
        fin.read(buf, float_size*3);
        float v3x = *reinterpret_cast<float*>(buf);
        float v3y = *reinterpret_cast<float*>(buf + float_size);
        float v3z = *reinterpret_cast<float*>(buf + float_size * 2);
        assert(fin.good());

        // attribute (2 bytes), not sure what purpose they serve.
        fin.read(buf, 2);

        Vector3F normal(nx, ny, nz);
        Vector3F v1(v1x, v1y, v1z);
        Vector3F v2(v2x, v2y, v2z);
        Vector3F v3(v3x, v3y, v3z);

        m_facet_normals.push_back(normal);
        m_vertices.push_back(v1);
        m_vertices.push_back(v2);
        m_vertices.push_back(v3);

        size_t v_idx= i*3;
        Vector3I face(v_idx, v_idx+1, v_idx+2);
        m_faces.push_back(face);

        assert(fin.good());
        if (!fin.good()) return false;
    }

    fin.close();
    return true;
}

void STLParser::merge_identical_vertices() {
    Float diagonal_length = compute_bbox_diagonal_length();
    size_t num_vertices = m_vertices.size();

    typedef Zhou::HashGrid::HashItem VertexSet;
    Zhou::HashGrid::Ptr hash_grid =
        Zhou::HashGrid::create(1e-3 * diagonal_length);

    size_t count=0;
    std::vector<int> index_map(num_vertices, -1);
    std::vector<const Vector3F*> vertex_array(num_vertices);
    for (VertexList::iterator itr = m_vertices.begin();
            itr != m_vertices.end(); itr++) {
        hash_grid->insert(count, *itr);
        vertex_array[count] = &*itr;
        count++;
    }

    count=0;
    size_t v_count=0;
    for (VertexList::iterator itr = m_vertices.begin();
            itr != m_vertices.end(); itr++) {
        if (index_map[count] == -1) {
            const VertexSet* nearby_vts = hash_grid->get_items(*itr);
            assert(nearby_vts != NULL);
            for (VertexSet::const_iterator jtr = nearby_vts->begin();
                    jtr != nearby_vts->end(); jtr++) {
                Float dist = (*itr - *vertex_array[*jtr]).norm();
                if (dist == 0) {
                    assert(*jtr >= count);
                    index_map[*jtr] = v_count;
                }
            }
            v_count++;
        }
        count++;
    }

    std::vector<const Vector3F*> vertex_merged_array(v_count, NULL);
    for(size_t i=0; i<num_vertices; i++) {
        assert(index_map[i] != -1);
        if (vertex_merged_array[index_map[i]] == NULL)
            vertex_merged_array[index_map[i]] = vertex_array[i];
    }

    VertexList v_list;
    for (size_t i=0; i<v_count; i++) {
        v_list.push_back(*vertex_merged_array[i]);
    }
    m_vertices = v_list;

    for (FaceList::iterator itr = m_faces.begin();
            itr != m_faces.end(); itr++) {
        for (size_t i=0; i<3; i++) {
            (*itr)[i] = index_map[(*itr)[i]];
        }
    }
}

void STLParser::validate_normals() {
    const Float EPS = 1e-16;
    bool valid = true;
    for (NormalList::iterator itr = m_facet_normals.begin();
            itr != m_facet_normals.end(); itr++) {
        Float n_norm = itr->norm();
        if (n_norm < 1.0 - EPS || n_norm > 1.0 + EPS) {
            valid = false;
            break;
        }
    }

    if (!valid) {
        m_facet_normals.clear();
    }
}

bool STLParser::has_normal() const {
    return !m_facet_normals.empty();
}

Float STLParser::compute_bbox_diagonal_length() const {
    const Float max_float = std::numeric_limits<Float>::max();
    const Float min_float = std::numeric_limits<Float>::min();
    Vector3F b_max(min_float, min_float, min_float),
             b_min(max_float, max_float, max_float);
    for (VertexList::const_iterator itr = m_vertices.begin();
            itr != m_vertices.end(); itr++) {
        const Vector3F& v = *itr;
        b_max[0] = std::max(v[0], b_max[0]);
        b_max[1] = std::max(v[1], b_max[1]);
        b_max[2] = std::max(v[2], b_max[2]);

        b_min[0] = std::min(v[0], b_min[0]);
        b_min[1] = std::min(v[1], b_min[1]);
        b_min[2] = std::min(v[2], b_min[2]);
    }

    return (b_max - b_min).norm();
}
