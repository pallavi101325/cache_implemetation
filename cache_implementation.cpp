#ifndef CACHES_CACHEIMPL_HPP
#define CACHES_CACHEIMPL_HPP

#include <unordered_map>
#include <list>
#include <stdexcept>


namespace CacheImpl {
template <typename K, typename V> class Cache {
private:
  std::size_t m_capacity;

public:
  explicit Cache(std::size_t capacity) : m_capacity(capacity) {}

  virtual ~Cache() = default;

  size_t getCapacity() const { return m_capacity; }

  void setCapacity(size_t capacity) { m_capacity = capacity; }

  virtual V get(const K &key) = 0;

  virtual void put(const K &key, const V &value) = 0;

  virtual void clear() = 0;
};

template <typename K, typename V, typename Key_Hash = std::hash<K>>
class LIFOCache : public Cache<K, V> {
private:
  std::list<std::pair<K, V>> m_list;
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Key_Hash>
      m_hashmap;

public:
  explicit LIFOCache(std::size_t capacity) : Cache<K, V>(capacity) {}

  V get(const K &key) override {
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      throw std::invalid_argument(
          "Key is not found!"); // throw an exception that indicates 'not found'
    }
    // Return 'value' from the pair
    return iter->second->second;
  }

   void put(const K &key, const V &value) override {
    // Corner case:
    if (Cache<K, V>::getCapacity() == 0) {
      return;
    }
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      if (m_hashmap.size() == Cache<K, V>::getCapacity()) {
        // The cache is full, we need to erase the last item from 'm_list' and
        // update the hash map (Last In First Out)
        m_hashmap.erase(m_list.back().first);
        m_list.pop_back();
      }
      m_list.emplace_back(std::make_pair(key, value));
      m_hashmap[key] = (--m_list.end());
    } else {
      iter->second->second = value;
    }
  }

  void clear() override {
    m_list.clear();
    m_hashmap.clear();
  }
};

template <typename K, typename V, typename Key_Hash = std::hash<K>>
class FIFOCache : public Cache<K, V> {
private:
  std::list<std::pair<K, V>> m_list;
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Key_Hash>
      m_hashmap;

public:
  explicit FIFOCache(std::size_t capacity) : Cache<K, V>(capacity) {}

  V get(const K &key) override {
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      throw std::invalid_argument(
          "Key is not found!"); // throw an exception that indicates 'not found'
    }
    // Return 'value' from the pair
    return iter->second->second;
  }

  void put(const K &key, const V &value) override {
    // Corner case:
    if (Cache<K, V>::getCapacity() == 0) {
      return;
    }
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      if (m_hashmap.size() == Cache<K, V>::getCapacity()) {
        // The cache is full, we need to erase the front item from 'm_list' and
        // update the hash map (First In First Out)
        m_hashmap.erase(m_list.front().first);
        m_list.pop_front();
      }
      m_list.emplace_back(std::make_pair(key, value));
      m_hashmap[key] = (--m_list.end());
    } else {
      iter->second->second = value;
    }
  }

  void clear() override {
    m_list.clear();
    m_hashmap.clear();
  }
};


template <typename K, typename V, typename Key_Hash = std::hash<K>>
class LRUCache : public Cache<K, V> {
private:
  std::list<std::pair<K, V>> m_list;
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Key_Hash>
      m_hashmap;

public:
  explicit LRUCache(std::size_t capacity) : Cache<K, V>(capacity) {}

  V get(const K &key) override {
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      throw std::invalid_argument(
          "Key is not found!"); // throw an exception that indicates 'not found'
    }
    // Otherwise, push the (key, value) pair to the front of 'm_list' and update
    // the hash map
    m_list.emplace_front(std::make_pair(key, iter->second->second));
    m_list.erase(iter->second);
    iter->second = m_list.begin();
    // Return 'value' from the pair
    return m_list.begin()->second;
  }

  void put(const K &key, const V &value) override {
    // Corner case:
    if (Cache<K, V>::getCapacity() == 0) {
      return;
    }
    // We check if 'key' is in the hash map
    auto iter = m_hashmap.find(key);
    if (iter == m_hashmap.end()) {
      if (m_hashmap.size() == Cache<K, V>::getCapacity()) {
        // The cache is full, we need to erase the least recently used item from
        // 'm_list' and update the hash map
        m_hashmap.erase(m_list.back().first);
        m_list.pop_back();
      }
      m_list.emplace_front(std::make_pair(key, value));
      m_hashmap[key] = m_list.begin();
    } else {
      m_list.erase(iter->second);
      m_list.emplace_front(std::make_pair(key, value));
      iter->second = m_list.begin();
    }
  }

  void clear() override {
    m_list.clear();
    m_hashmap.clear();
  }
};
} 

#endif