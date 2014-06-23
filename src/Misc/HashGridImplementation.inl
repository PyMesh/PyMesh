#include <cassert>
#include <Core/Exception.h>

template<typename Trait>
HashGridImplementation<Trait>::HashGridImplementation(Float cell_size) : HashGrid(cell_size) {
    m_hash_map = Trait::create_map();
}

template<typename Trait>
bool HashGridImplementation<Trait>::insert(int obj_id, const VectorF& coordinates) {
    HashKey key = convert_to_key(coordinates);
    return insert_key(obj_id, key);
}

template<typename Trait>
bool HashGridImplementation<Trait>::insert_bbox(int obj_id, const MatrixF& shape) {
    assert(shape.cols() == 3 || shape.cols() == 2);
    Float X_min = shape.col(0).minCoeff();
    Float Y_min = shape.col(1).minCoeff();
    Float Z_min = 0.0;
    Float X_max = shape.col(0).maxCoeff();
    Float Y_max = shape.col(1).maxCoeff();
    Float Z_max = 0.0;
    if (shape.cols() == 3) {
        Z_min = shape.col(2).minCoeff();
        Z_max = shape.col(2).maxCoeff();
    }
    HashKey min_key = convert_to_key(X_min, Y_min, Z_min);
    HashKey max_key = convert_to_key(X_max, Y_max, Z_max);

    bool success = true;
    for (typename HashKey::ValueType x=min_key[0]; x<=max_key[0]; x+=1) {
        for (typename HashKey::ValueType y=min_key[1]; y<=max_key[1]; y+=1) {
            for (typename HashKey::ValueType z=min_key[2]; z<=max_key[2]; z+=1) {
                HashKey cur_key(x, y, z);
                bool r = insert_key(obj_id, cur_key);
                success &= r;
            }
        }
    }
    return success;
}

template<typename Trait>
bool HashGridImplementation<Trait>::insert_batch(int obj_id, const MatrixFr& points) {
    size_t num_pts = points.rows();
    bool success = true;
    for (size_t i=0; i<num_pts; i++) {
        bool r = insert(obj_id, points.row(i));
        success &= r;
    }
    return success;
}

template<typename Trait>
bool HashGridImplementation<Trait>::insert_multiple(const VectorI& obj_ids, const MatrixFr& points) {
    size_t num_pts = points.rows();
    if (obj_ids.size() != num_pts) {
        std::stringstream err_msg;
        err_msg << "Number of object IDs does not match number of points: "
            << obj_ids.size() << " != " << num_pts;
        throw RuntimeError(err_msg.str());
    }
    bool success = true;
    for (size_t i=0; i<num_pts; i++) {
        bool r = insert(obj_ids[i], points.row(i));
        success &= r;
    }
    return success;
}

template<typename Trait>
bool HashGridImplementation<Trait>::remove(int obj_id, const VectorF& coordinates) {
    HashKey key = convert_to_key(coordinates);
    typename HashMap::iterator itr = m_hash_map->find(key);
    if (itr == m_hash_map->end()) return false;

    std::size_t num_erased = itr->second.erase(obj_id);

    if (itr->second.empty()) {
        m_hash_map->erase(key);
    }
    return num_erased != 0;
}

template<typename Trait>
bool HashGridImplementation<Trait>::occupied(int obj_id, const VectorF& coordinates) const {
    HashKey key = convert_to_key(coordinates);

    typename HashMap::const_iterator itr = m_hash_map->find(key);
    if (itr == m_hash_map->end()) return false;

    typename HashItem::const_iterator i = itr->second.find(obj_id);
    return i!=itr->second.end();
}

template<typename Trait>
VectorI HashGridImplementation<Trait>::get_items_near_point(const VectorF& coordinates) {
    HashItem nearby_set = Trait::get_default_item();
    HashKey center_key = convert_to_key(coordinates);

    for (int i=-1; i<2; i++) {
        for (int j=-1; j<2; j++) {
            for (int k=-1; k<2; k++) {
                HashKey key(center_key[0] + i, center_key[1] + j, center_key[2] + k);
                typename HashMap::const_iterator itr = m_hash_map->find(key);
                if (itr != m_hash_map->end()) {
                    nearby_set.insert(itr->second.begin(), itr->second.end());
                }
            }
        }
    }

    VectorI result(nearby_set.size());
    std::copy(nearby_set.begin(), nearby_set.end(), result.data());
    return result;
}

template<typename Trait>
typename HashGridImplementation<Trait>::HashKey HashGridImplementation<Trait>::convert_to_key(const VectorF& value) const {
    switch (value.size()) {
        case 2:
            return convert_to_key(value[0], value[1], 0.0);
            break;
        case 3:
            return convert_to_key(value[0], value[1], value[2]);
            break;
        default:
            throw NotImplementedError("Only 2D and 3D hash grid are supported.");
    }
}

template<typename Trait>
typename HashGridImplementation<Trait>::HashKey HashGridImplementation<Trait>::convert_to_key(Float x, Float y, Float z) const {
    HashKey key(
            int(x / m_cell_size),
            int(y / m_cell_size),
            int(z / m_cell_size));
    return key;
}

template<typename Trait>
bool HashGridImplementation<Trait>::insert_key(int obj_id, HashGridImplementation<Trait>::HashKey& key) {
    typename HashMap::iterator itr = m_hash_map->find(key);
    if (itr == m_hash_map->end()) {
        HashItem item = Trait::get_default_item();
        item.insert(obj_id);
        typename HashMap::value_type data(key, item);
        return m_hash_map->insert(data).second;
    } else {
        return itr->second.insert(obj_id).second;
    }
}

