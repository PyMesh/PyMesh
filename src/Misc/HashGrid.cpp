#include "HashGrid.h"

#include <cassert>
#include <sstream>
#include <limits>
#include <iostream>

#include <Core/Exception.h>

using namespace Zhou;

HashGrid::Ptr HashGrid::create(Float cell_size) {
    return Ptr(new HashGrid(cell_size));
}

HashGrid::HashGrid(Float cell_size) :
    m_cell_size(cell_size) {
        const int int_max = std::numeric_limits<int>::max();
#if HASH_TYPE==2
	m_hashMap.set_empty_key(HashKey(int_max, int_max, int_max));
#endif
#if HASH_TYPE!=0
	m_hashMap.set_deleted_key(HashKey(int_max-1, int_max-1, int_max-1));
#endif
}

bool HashGrid::insert(int obj_id, const VectorF& coordinates) {
    HashKey key = convert_to_key(coordinates);
    return insert_key(obj_id, key);
}

bool HashGrid::insert_bbox(int obj_id, const MatrixF& shape) {
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
    for (HashKey::ValueType x=min_key[0]; x<=max_key[0]; x+=1) {
        for (HashKey::ValueType y=min_key[1]; y<=max_key[1]; y+=1) {
            for (HashKey::ValueType z=min_key[2]; z<=max_key[2]; z+=1) {
                HashKey cur_key(x, y, z);
                bool r = insert_key(obj_id, cur_key);
                success &= r;
            }
        }
    }
    return success;
}

bool HashGrid::insert_batch(int obj_id, const MatrixF& points) {
    size_t num_pts = points.rows();
    bool success = true;
    for (size_t i=0; i<num_pts; i++) {
        bool r = insert(obj_id, points.row(i));
        success &= r;
    }
    return success;
}

bool HashGrid::insert_multiple(const VectorI& obj_ids, const MatrixF& points) {
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

bool HashGrid::remove(int obj_id, const VectorF& coordinates) {
    HashKey key = convert_to_key(coordinates);
    HashMap::iterator itr = m_hashMap.find(key);
    if (itr == m_hashMap.end()) return false;

    std::size_t num_erased = itr->second.erase(obj_id);

    if (itr->second.empty()) {
        m_hashMap.erase(key);
    }
    return num_erased != 0;
}

bool HashGrid::occupied(int obj_id, const VectorF& coordinates) const {
	HashKey key = convert_to_key(coordinates);

	HashMap::const_iterator itr = m_hashMap.find(key);
	if (itr == m_hashMap.end()) return false;

	HashItem::const_iterator i = itr->second.find(obj_id);
	return i!=itr->second.end();
}

const HashGrid::HashItem* HashGrid::get_items(const VectorF& coordinates) {
    HashKey key = convert_to_key(coordinates);
    HashMap::const_iterator itr = m_hashMap.find(key);
    if (itr == m_hashMap.end()) return NULL;

    return &(itr->second);
}

VectorI HashGrid::get_items_near_point(const VectorF& coordinates) {
    HashGrid::HashItem nearby_set;
#if HASH_TYPE==2
    nearby_set.set_empty_key(-1);
#endif
#if HASH_TYPE!=0
    nearby_set.set_deleted_key(-2);
#endif

    const Float half_cell_size = m_cell_size * 0.5;
    VectorF offset(3);
    VectorF point = VectorF::Zero(3);
    point.segment(0, coordinates.size()) = coordinates;
    for (int i=-1; i<2; i++) {
        offset[0] = i * half_cell_size;
        for (int j=-1; j<2; j++) {
            offset[1] = j * half_cell_size;
            for (int k=-1; k<2; k++) {
                offset[2] = k * half_cell_size;
                const VectorF p = point + offset;
                const HashGrid::HashItem* item = get_items(p);
                if (item != NULL) {
                    nearby_set.insert(item->begin(), item->end());
                }
            }
        }
    }

    VectorI result(nearby_set.size());
    std::copy(nearby_set.begin(), nearby_set.end(), result.data());
    return result;
}

HashGrid::HashKey HashGrid::convert_to_key(const VectorF& value) const {
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

HashGrid::HashKey HashGrid::convert_to_key(Float x, Float y, Float z) const {
    HashKey key(
            int(x / m_cell_size),
            int(y / m_cell_size),
            int(z / m_cell_size));
    return key;
}

bool HashGrid::insert_key(int obj_id, HashGrid::HashKey& key) {
    HashMap::iterator itr = m_hashMap.find(key);
    if (itr == m_hashMap.end()) {
        HashItem item;
#if HASH_TYPE==2
        item.set_empty_key(-1);
#endif
#if HASH_TYPE!=0
        item.set_deleted_key(-2);
#endif
        item.insert(obj_id);
        return m_hashMap.insert(HashMap::value_type(key, item)).second;
    } else {
        return itr->second.insert(obj_id).second;
    }
}

void HashGrid::print_hash_efficiency() const {
    size_t ave_collisions=0;
    size_t num_occupied_cells = 0;
    for (HashMap::const_iterator itr=m_hashMap.begin();
            itr != m_hashMap.end(); itr++) {
        size_t num_collisions = m_hashMap.count(itr->first);
        if (num_collisions != 0) {
            num_occupied_cells++;
            ave_collisions += num_collisions;
        }
    }

    std::cout << "Bucket count: " << bucket_count() << std::endl;
    std::cout << "Occupancy rate: " <<
        double(num_occupied_cells) / double(bucket_count()) << std::endl;
    std::cout << "Ave collisions: " <<
        double(ave_collisions) / double(num_occupied_cells) << std::endl;
}
