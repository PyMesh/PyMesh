#include "HashGrid.h"

#include <cassert>
#include <sstream>
#include <limits>
#include <iostream>

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

bool HashGrid::insert(int obj_id, const Vector3F& coordinates) {
    HashKey key = convert_to_key(coordinates);
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
        m_hashMap.insert(HashMap::value_type(key, item));
    } else {
        return itr->second.insert(obj_id).second;
    }
}

bool HashGrid::remove(int obj_id, const Vector3F& cooridnates) {
    HashKey key = convert_to_key(cooridnates);
    HashMap::iterator itr = m_hashMap.find(key);
    if (itr == m_hashMap.end()) return false;

    std::size_t num_erased = itr->second.erase(obj_id);

    if (itr->second.empty()) {
        m_hashMap.erase(key);
    }
    return num_erased != 0;
}

bool HashGrid::occupied(int obj_id, const Vector3F& coordinates) const {
	HashKey key = convert_to_key(coordinates);

	HashMap::const_iterator itr = m_hashMap.find(key);
	if (itr == m_hashMap.end()) return false;

	HashItem::const_iterator i = itr->second.find(obj_id);
	return i!=itr->second.end();
}

const HashGrid::HashItem* HashGrid::get(const Vector3F& coordinates) {
    HashKey key = convert_to_key(coordinates);
    HashMap::const_iterator itr = m_hashMap.find(key);
    if (itr == m_hashMap.end()) return NULL;

    return &(itr->second);
}

HashGrid::HashKey HashGrid::convert_to_key(const Vector3F& value) const {
    HashKey key(
            int(value[0] / m_cell_size),
            int(value[1] / m_cell_size),
            int(value[2] / m_cell_size));
    return key;
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
