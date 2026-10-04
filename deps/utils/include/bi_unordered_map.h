#ifndef _BI_UNORDERED_MAP_
#define _BI_UNORDERED_MAP_

#include <iostream>
#include <unordered_map>
#include <initializer_list>

template <class K,
          class V,
          class HashK = std::hash<K>,
          class HashV = std::hash<V>,
          class EqualK = std::equal_to<K>,
          class EqualV = std::equal_to<V>,
          class AllocatorK = std::allocator<std::pair<const K, V>>,
          class AllocatorV = std::allocator<std::pair<const V, K>>>
class bi_unordered_map
{
private:
  using KeyValueUnorderedMap =   std::unordered_map<K, V>;
  using ValueKeyUnorderedMap = std::unordered_map<V, K>;
  KeyValueUnorderedMap m_keyValuedMap;
  ValueKeyUnorderedMap m_valueKeyMap;

public:
  using iterator =        typename KeyValueUnorderedMap::iterator;
  using viterator =       typename ValueKeyUnorderedMap::iterator;
  using const_iterator =  typename KeyValueUnorderedMap::const_iterator;
  using const_viterator = typename ValueKeyUnorderedMap::const_iterator;

  bi_unordered_map(std::size_t bucketCount = 0,
                   HashK hashK = HashK(),
                   HashV hashV = HashV(),
                   EqualK equalK = EqualK(),
                   EqualV equalV = EqualV(),
                   AllocatorK allocatorK = AllocatorK(),
                   AllocatorV allocatorV = AllocatorV())
    : m_keyValuedMap(bucketCount, hashK, equalK, allocatorK),
      m_valueKeyMap(bucketCount, hashV, equalV, allocatorV)
  { }

  // Initializer list constructor
  bi_unordered_map(std::initializer_list<std::pair<const K, V>> list,
                   std::size_t bucketCount = 0,
                   HashK hashK = HashK(),
                   HashV hashV = HashV(),
                   EqualK equalK = EqualK(),
                   EqualV equalV = EqualV(),
                   AllocatorK allocatorK = AllocatorK(),
                   AllocatorV allocatorV = AllocatorV())
    : m_keyValuedMap(bucketCount, hashK, equalK, allocatorK),
      m_valueKeyMap(bucketCount, hashV, equalV, allocatorV)
  {
    for (const auto &pair : list)
    {
      insert(pair);
    }
  }

  std::pair<iterator, bool> insert(const std::pair<K, V> &pair)
  {
    auto rValue = m_keyValuedMap.insert(pair);
    if (rValue.second)
    {
      m_valueKeyMap.insert({pair.second, pair.first});
    }
    return rValue;
  }

  std::pair<iterator, bool> vinsert(const std::pair<V, K> &pair)
  {
    auto rValue = m_valueKeyMap.insert(pair);
    if (rValue.second)
    {
      m_keyValuedMap.insert({pair.second, pair.first});
    }
    return rValue;
  }

  V &operator[](const K &key)
  {
    return m_keyValuedMap[key];
  }

  // std::out_of_range if the container does not have an element with the specified key.
  V &at(const K &key)
  {
    return m_keyValuedMap.at(key);
  }

  // std::out_of_range if the container does not have an element with the specified key.
  const V &at(const K &key) const
  {
    return m_keyValuedMap.at(key);
  }

  // std::out_of_range if the container does not have an element with the specified value.
  K &vat(const V &value)
  {
    return m_valueKeyMap.at(value);
  }

  // std::out_of_range if the container does not have an element with the specified value.
  const K &vat(const V &value) const
  {
    return m_valueKeyMap.at(value);
  }

  size_t erase(const K &key)
  {
    auto it = m_keyValuedMap.find(key);
    if (it == m_keyValuedMap.end()) return 0;

    m_valueKeyMap.erase(it->second);
    m_keyValuedMap.erase(it);
    return 1;
  }

  size_t verase(const V &value)
  {
    auto it = m_valueKeyMap.find(value);
    if (it == m_valueKeyMap.end()) return 0;

    m_keyValuedMap.erase(it->second);
    m_valueKeyMap.erase(it);
    return 1;
  }

  size_t erase(const_iterator &it)
  {
    if (it == m_keyValuedMap.end()) return 0;

    m_valueKeyMap.erase(it->second);
    m_keyValuedMap.erase(it);
    return 1;
  }

  size_t verase(const_viterator &vit)
  {
    if (vit == m_valueKeyMap.end()) return 0;

    m_keyValuedMap.erase(vit->second);
    m_valueKeyMap.erase(vit);
    return 1;
  }

  // Search in key based map
  iterator find(const K &key)
  {
    return m_keyValuedMap.find(key);
  }

  // Search in key based map
  const_iterator find(const K &key) const
  {
    return m_keyValuedMap.find(key);
  }

  // Search in value based map
  viterator vfind(const V &value)
  {
    return m_valueKeyMap.find(value);
  }

  // Search in value based map
  const_viterator vfind(const V &value) const
  {
    return m_valueKeyMap.find(value);
  }

  // size() method
  size_t size() const
  {
    return m_keyValuedMap.size();
  }

  // empty() method
  bool empty() const
  {
    return m_keyValuedMap.empty();
  }

  // clear() method
  void clear()
  {
    m_keyValuedMap.clear();
    m_valueKeyMap.clear();
  }

  // Iterators
  iterator begin() { return m_keyValuedMap.begin(); }
  iterator end() { return m_keyValuedMap.end(); }
  const_iterator begin() const { return m_keyValuedMap.begin(); }
  const_iterator end() const { return m_keyValuedMap.end(); }

  // Const Iterators
  viterator vbegin() { return m_valueKeyMap.begin(); }
  viterator vend() { return m_valueKeyMap.end(); }
  const_viterator vbegin() const { return m_valueKeyMap.begin(); }
  const_viterator vend() const { return m_valueKeyMap.end(); }
};

#endif // _BI_UNORDERED_MAP_
