#ifndef MAP_HPP
#define MAP_HPP

#include "Tree.hpp"
#include <utility>

template <class Key, class T, class Compare, bool AreKeysUnique>
class MapBase {
public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using allocator_type = std::allocator<value_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

private:
  struct SelectFirst {
    const Key& operator()(const_reference p) {
      return p.first;
    }
  };

  using Tree = RbTree<Key, value_type, SelectFirst, Compare, AreKeysUnique>;
  Tree tree;

public:
  MapBase() = default;
  MapBase(const Compare& comp) : tree(comp) {}
  template <class InputIterator>
  MapBase(InputIterator first, InputIterator last, Compare comp = Compare()) :
      tree(comp) {
    while (first != last)
      tree.insert(*first++);
  }
  MapBase(const MapBase&) = default;
  MapBase(MapBase&&) = default;
  MapBase(std::initializer_list<value_type> init, Compare comp = Compare()) :
      tree(comp) {
    for (auto&& e : init)
      tree.insert(e);
  }
  ~MapBase() = default;
  MapBase& operator=(const MapBase&) = default;
  MapBase& operator=(MapBase&&) = default;
  MapBase& operator=(std::initializer_list<value_type> init) {
    tree.clear();
    for (auto&& e : init)
      tree.insert(e);
  }
  allocator_type get_allocator() const {
    return allocator_type();
  }
  mapped_type& at(const key_type& key)
  requires AreKeysUnique
  {
    iterator i = tree.lower_bound(key);
    return i->second;
  }
  mapped_type& at(const key_type& key) const
  requires AreKeysUnique
  {
    iterator i = tree.lower_bound(key);
    return i->second;
  }
  mapped_type& operator[](const key_type& key)
  requires AreKeysUnique
  {
    iterator i = tree.lower_bound(key);
    if (i == tree.end() || key_comp()(key, i->first))
      i = tree.insert_hint(i, value_type(std::piecewise_construct,
                                         std::tuple<const key_type&>(key),
                                         std::tuple<>()));
    return i->second;
  }
  mapped_type& operator[](key_type&& key)
  requires AreKeysUnique
  {
    iterator i = tree.lower_bound(key);
    if (i == tree.end() || key_comp()(key, i->first))
      i = tree.insert_hint(i, value_type(std::piecewise_construct,
                                         std::forward_as_tuple(std::move(key)),
                                         std::tuple<>()));
    return i->second;
  }

  using iterator = Tree::iterator;
  using const_iterator = Tree::const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    return tree.begin();
  }
  const_iterator begin() const {
    return tree.begin();
  }
  const_iterator cbegin() const {
    return tree.begin();
  }
  iterator end() {
    return tree.end();
  }
  const_iterator end() const {
    return tree.end();
  }
  const_iterator cend() const {
    return tree.end();
  }
  reverse_iterator rbegin() {
    return std::reverse_iterator(tree.end());
  }
  const_reverse_iterator rbegin() const {
    return std::reverse_iterator(tree.end());
  }
  const_reverse_iterator crbegin() const {
    return std::reverse_iterator(tree.end());
  }
  reverse_iterator rend() {
    return std::reverse_iterator(tree.begin());
  }
  const_reverse_iterator rend() const {
    return std::reverse_iterator(tree.begin());
  }
  const_reverse_iterator crend() const {
    return std::reverse_iterator(tree.begin());
  }
  bool empty() const {
    return tree.size() == 0;
  }
  size_type size() const {
    return tree.size();
  }
  size_type max_size() const {
    return std::numeric_limits<difference_type>::max() / sizeof(value_type);
  }
  void clear() {
    tree.clear();
  }
  auto insert(const value_type& value) {
    return tree.insert(value);
  }
  auto insert(value_type&& value) {
    return tree.insert(std::move(value));
  }
  template <class Pair>
  requires std::is_constructible_v<value_type, Pair>
  auto insert(Pair&& pair) {
    return tree.insert(value_type(std::forward<Pair>(pair)));
  }
  iterator insert(const_iterator pos, const value_type& value) {
    return tree.insert_hint(pos, value);
  }
  iterator insert(const_iterator pos, value_type&& value) {
    return tree.insert_hint(pos, std::move(value));
  }
  template <class Pair>
  requires std::is_constructible_v<value_type, Pair>
  iterator insert(const_iterator pos, Pair&& pair) {
    return tree.insert_hint(pos, value_type(std::forward<Pair>(pair)));
  }
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    while (first != last)
      tree.insert(first++);
  }
  void insert(std::initializer_list<value_type> init) {
    for (auto&& e : init)
      tree.insert(e);
  }
  template <class... Args>
  auto emplace(Args... args) {
    return tree.insert(value_type(std::forward<Args>(args)...));
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return tree.insert(hint, value_type(std::forward<Args>(args)...));
  }
  iterator erase(iterator pos) {
    return tree.erase(pos);
  }
  iterator erase(const_iterator pos) {
    return tree.erase(pos);
  }
  iterator erase(const_iterator first, const_iterator last) {
    while (first != last)
      tree.erase(first++);
    return iterator(last.node);
  }
  size_type erase(const Key& key) {
    auto p = tree.equal_range(key);
    const size_type old_size = tree.size();
    while (p.first != p.second)
      tree.erase(p.first++);
    return old_size - tree.size();
  }
  void swap(MapBase& other) {
    std::swap(*this, other);
  }
  iterator find(const Key& key) {
    return tree.find(key);
  }
  const_iterator find(const Key& key) const {
    return tree.find(key);
  }
  size_type count(const Key& key) const {
    return tree.count(key);
  }
  std::pair<iterator, iterator> equal_range(const Key& key) {
    return tree.equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
    return tree.equal_range(key);
  }
  iterator upper_bound(const Key& key) {
    return tree.upper_bound(key);
  }
  const_iterator upper_bound(const Key& key) const {
    return tree.upper_bound(key);
  }
  iterator lower_bound(const Key& key) {
    return tree.lower_bound(key);
  }
  const_iterator lower_bound(const Key& key) const {
    return tree.lower_bound(key);
  }

  class value_compare {
  protected:
    Compare comp;
    value_compare(Compare c) : comp(c) {}

  public:
    bool operator()(const_reference lhs, const_reference rhs) {
      return comp(lhs, rhs);
    }
  };
  key_compare key_comp() const {
    return tree.key_comp();
  }
  value_compare value_comp() const {
    return value_compare(tree.key_comp());
  }
  bool operator==(const MapBase& other) {
    return tree == other.tree;
  }
  auto operator<=>(const MapBase& other) {
    return tree <=> other.tree;
  }
};

template <class Key, class T, class Compare = std::less<Key>>
using Map = MapBase<Key, T, Compare, true>;
template <class Key, class T, class Compare = std::less<Key>>
using MultiMap = MapBase<Key, T, Compare, false>;

#endif
