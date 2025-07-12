#ifndef SET_HPP
#define SET_HPP

#include "Tree.hpp"

template <class Key, class Compare, bool AreKeysUnique>
class BasicSet {
  using Tree = RbTree<Key, Key, std::identity, Compare, AreKeysUnique>;
  Tree tree;

public:
  using key_type = Key;
  using value_type = Key;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using value_compare = Compare;
  using allocator_type = std::allocator<value_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = Tree::const_iterator;
  using const_iterator = Tree::const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  BasicSet() = default;
  ~BasicSet() = default;
  BasicSet(const BasicSet& other) = default;
  BasicSet(BasicSet&& other) = default;
  BasicSet& operator=(const BasicSet&) = default;
  BasicSet& operator=(BasicSet&&) = default;

  explicit BasicSet(const Compare& compare) : tree(compare) {};
  template <class InputIterator>
  BasicSet(InputIterator first, InputIterator last,
           const Compare& compare = Compare()) :
      tree(compare) {
    while (first != last)
      tree.insert(*first++);
  }
  BasicSet(std::initializer_list<value_type> init,
           const Compare& compare = Compare()) :
      tree(compare) {
    for (auto&& e : init)
      tree.insert(e);
  }
  BasicSet& operator=(std::initializer_list<value_type> init) {
    tree.clear();
    for (auto&& e : init)
      tree.insert(e);
  }
  allocator_type get_allocator() const {
    return allocator_type();
  }

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
    return std::allocator_traits<std::allocator<key_type>>::max_size();
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
  iterator insert(const_iterator pos, const value_type& value) {
    return tree.insert_hint(pos, value);
  }
  iterator insert(const_iterator pos, value_type&& value) {
    return tree.insert_hint(pos, std::move(value));
  }
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    while (first != last)
      tree.insert(*first++);
  }
  void insert(std::initializer_list<value_type> init) {
    for (auto&& e : init)
      tree.insert(e);
  }
  template <class... Args>
  auto emplace(Args&&... args) {
    return insert(value_type(std::forward<Args>(args)...));
  }
  template <class... Args>
  auto emplace_hint(const_iterator hint, Args&&... args) {
    return insert(hint, value_type(std::forward<Args>(args)...));
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
  void swap(BasicSet& other) {
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
  value_compare value_comp() const {
    return tree.key_comp();
  }
  value_compare key_comp() const {
    return tree.key_comp();
  }
  bool operator==(const BasicSet& other) {
    return tree == other.tree;
  }
  auto operator<=>(const BasicSet& other) {
    return tree <=> other.tree;
  }
};

template <class Key, class Compare = std::less<Key>>
using Set = BasicSet<Key, Compare, true>;
template <class Key, class Compare = std::less<Key>>
using MultiSet = BasicSet<Key, Compare, false>;

#endif
