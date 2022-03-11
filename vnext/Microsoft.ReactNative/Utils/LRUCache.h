// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// Credit: https://github.com/lamerman/cpp-lru-cache

#pragma once

#include <unordered_map>
#include <list>
#include <cstddef>
#include <stdexcept>

namespace react::uwp {

template<typename TKey, typename TValue>
class LRUCache {
public:
	typedef typename std::pair<TKey, TValue> TKeyValuePair;
	typedef typename std::list<TKeyValuePair>::iterator TListIterator;

	LRUCache(size_t maxSize) :
		m_maxSize(maxSize) {
	}

	void Put(const TKey& key, const TValue& value);

	const TValue& Get(const TKey& key);

	bool Exists(const TKey& key) const;

	size_t Size() const;

private:
	std::list<TKeyValuePair> m_cacheItemsList;
	std::unordered_map<TKey, TListIterator> m_cacheItemsMap;
	size_t m_maxSize;
};

template<typename TKey, typename TValue>
void LRUCache<TKey, TValue>::Put(const TKey& key, const TValue& value) {
    auto it = m_cacheItemsMap.find(key);
    m_cacheItemsList.push_front(TKeyValuePair(key, value));
    if (it != m_cacheItemsMap.end()) {
        m_cacheItemsList.erase(it->second);
        m_cacheItemsMap.erase(it);
    }
    m_cacheItemsMap[key] = m_cacheItemsList.begin();

    if (m_cacheItemsMap.size() > m_maxSize) {
        auto last = m_cacheItemsList.end();
        last--;
        m_cacheItemsMap.erase(last->first);
        m_cacheItemsList.pop_back();
    }
}

template<typename TKey, typename TValue>
const TValue& LRUCache<TKey, TValue>::Get(const TKey& key) {
    auto it = m_cacheItemsMap.find(key);
    if (it == m_cacheItemsMap.end()) {
        throw std::range_error("There is no such key in cache");
    } else {
        m_cacheItemsList.splice(m_cacheItemsList.begin(), m_cacheItemsList, it->second);
        return it->second->second;
    }
}

template<typename TKey, typename TValue>
bool LRUCache<TKey, TValue>::Exists(const TKey& key) const {
    return m_cacheItemsMap.find(key) != m_cacheItemsMap.end();
}

template<typename TKey, typename TValue>
size_t LRUCache<TKey, TValue>::Size() const {
    return m_cacheItemsMap.size();
}

}
