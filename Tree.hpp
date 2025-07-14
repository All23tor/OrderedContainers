#ifndef STL_TREE_H
#define STL_TREE_H

#include <iterator>

namespace {
enum class Color : bool {
  Red,
  Black,
};

struct NodeBase {
  Color color;
  NodeBase* parent;
  NodeBase* left;
  NodeBase* right;

  NodeBase* minimum() {
    NodeBase* x = this;
    while (x->left)
      x = x->left;
    return x;
  }

  NodeBase* maximum() {
    NodeBase* x = this;
    while (x->right)
      x = x->right;
    return x;
  }
};

template <class Val>
struct Node : NodeBase {
  template <class... Args>
  Node(Args&&... args) : val(std::forward<Args>(args)...) {}
  Val val;

  static constexpr auto up_cast(NodeBase* base) {
    return reinterpret_cast<Node*>(base);
  }

  static constexpr auto up_cast(const NodeBase* base) {
    return reinterpret_cast<const Node*>(base);
  }

  static void deep_erase(Node* x) {
    if (!x)
      return;
    deep_erase(up_cast(x->left));
    deep_erase(up_cast(x->right));
    delete x;
  }

  static Node* deep_copy(Node* x, NodeBase* parent) {
    if (!x)
      return nullptr;
    Node* node = new Node(x->val);
    node->parent = parent;
    node->right = deep_copy(up_cast(x->right), node);
    node->left = deep_copy(up_cast(x->left), node);
    node->color = x->color;
    return node;
  }
};

void rotate_left(NodeBase* x, NodeBase*& root) {
  NodeBase* const y = x->right;
  x->right = y->left;
  if (y->left)
    y->left->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->left = x;
  x->parent = y;
}

void rotate_right(NodeBase* x, NodeBase*& root) {
  NodeBase* const y = x->left;
  x->left = y->right;
  if (y->right)
    y->right->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  y->right = x;
  x->parent = y;
}

template <class Val>
struct Header {
  using Node = ::Node<Val>;

  NodeBase super_root;
  std::size_t node_count;
  Header() {
    root() = nullptr;
    leftmost() = &super_root;
    rightmost() = &super_root;
    super_root.color = ::Color::Red;
    node_count = 0;
  }

  Header(const Header& x) {
    root() = Node::deep_copy(Node::up_cast(x.root()), &super_root);
    if (root()) {
      root()->parent = &super_root;
      leftmost() = root()->minimum();
      rightmost() = root()->maximum();
    } else {
      leftmost() = &super_root;
      rightmost() = &super_root;
    }
    super_root.color = x.super_root.color;
    node_count = x.node_count;
  }

  Header(Header&& other) {
    root() = other.root();
    if (root()) {
      root()->parent = &super_root;
      leftmost() = other.leftmost();
      rightmost() = other.rightmost();
    } else {
      leftmost() = &super_root;
      rightmost() = &super_root;
    }
    super_root.color = other.super_root.color;
    node_count = other.node_count;

    other.root() = nullptr;
    other.leftmost() = &other.super_root;
    other.rightmost() = &other.super_root;
    other.super_root.color = ::Color::Red;
    other.node_count = 0;
  }

  ~Header() {
    Node::deep_erase(Node::up_cast(root()));
  }

  Header& operator=(Header&& other) {
    this->~Header();
    new (this) Header(std::move(other));
    return *this;
  }

  void clear() {
    this->~Header();
    new (this) Header();
  }

  auto&& root(this auto&& self) {
    return self.super_root.parent;
  }

  auto&& leftmost(this auto&& self) {
    return self.super_root.left;
  }

  auto&& rightmost(this auto&& self) {
    return self.super_root.right;
  }

  void insert(const bool insert_left, NodeBase* x, NodeBase* p) {
    x->parent = p;
    x->left = x->right = nullptr;
    x->color = Color::Red;

    if (insert_left) {
      p->left = x;
      if (p == &super_root) {
        root() = x;
        rightmost() = x;
      } else if (p == leftmost())
        leftmost() = x;
    } else {
      p->right = x;
      if (p == rightmost())
        rightmost() = x;
    }

    while (x != root() && x->parent->color == Color::Red) {
      NodeBase* const xpp = x->parent->parent;
      if (x->parent == xpp->left) {
        NodeBase* const y = xpp->right;
        if (y && y->color == Color::Red) {
          x->parent->color = Color::Black;
          y->color = Color::Black;
          xpp->color = Color::Red;
          x = xpp;
        } else {
          if (x == x->parent->right) {
            x = x->parent;
            rotate_left(x, root());
          }
          x->parent->color = Color::Black;
          xpp->color = Color::Red;
          rotate_right(xpp, root());
        }
      } else {
        NodeBase* const y = xpp->left;
        if (y && y->color == Color::Red) {
          x->parent->color = Color::Black;
          y->color = Color::Black;
          xpp->color = Color::Red;
          x = xpp;
        } else {
          if (x == x->parent->left) {
            x = x->parent;
            rotate_right(x, root());
          }
          x->parent->color = Color::Black;
          xpp->color = Color::Red;
          rotate_left(xpp, root());
        }
      }
    }
    root()->color = Color::Black;
    ++node_count;
  }

  void erase(NodeBase* z) {
    NodeBase* y = z;
    NodeBase* x{};
    NodeBase* x_parent{};

    if (!y->left)
      x = y->right;
    else if (!y->right)
      x = y->left;
    else {
      y = y->right;
      while (y->left)
        y = y->left;
      x = y->right;
    }
    if (y != z) {
      z->left->parent = y;
      y->left = z->left;
      if (y != z->right) {
        x_parent = y->parent;
        if (x)
          x->parent = y->parent;
        y->parent->left = x;
        y->right = z->right;
        z->right->parent = y;
      } else
        x_parent = y;
      if (root() == z)
        root() = y;
      else if (z->parent->left == z)
        z->parent->left = y;
      else
        z->parent->right = y;
      y->parent = z->parent;
      std::swap(y->color, z->color);
      y = z;

    } else {
      x_parent = y->parent;
      if (x)
        x->parent = y->parent;
      if (root() == z)
        root() = x;
      else if (z->parent->left == z)
        z->parent->left = x;
      else
        z->parent->right = x;
      if (leftmost() == z) {
        if (!z->right)
          leftmost() = z->parent;
        else
          leftmost() = x->minimum();
      }
      if (rightmost() == z) {
        if (!z->left)
          rightmost() = z->parent;
        else
          rightmost() = x->maximum();
      }
    }
    if (y->color != Color::Red) {
      while (x != root() && (!x || x->color == Color::Black))
        if (x == x_parent->left) {
          NodeBase* w = x_parent->right;
          if (w->color == Color::Red) {
            w->color = Color::Black;
            x_parent->color = Color::Red;
            rotate_left(x_parent, root());
            w = x_parent->right;
          }
          if ((!w->left || w->left->color == Color::Black) &&
              (!w->right || w->right->color == Color::Black)) {
            w->color = Color::Red;
            x = x_parent;
            x_parent = x_parent->parent;
          } else {
            if (!w->right || w->right->color == Color::Black) {
              w->left->color = Color::Black;
              w->color = Color::Red;
              rotate_right(w, root());
              w = x_parent->right;
            }
            w->color = x_parent->color;
            x_parent->color = Color::Black;
            if (w->right)
              w->right->color = Color::Black;
            rotate_left(x_parent, root());
            break;
          }
        } else {
          NodeBase* w = x_parent->left;
          if (w->color == Color::Red) {
            w->color = Color::Black;
            x_parent->color = Color::Red;
            rotate_right(x_parent, root());
            w = x_parent->left;
          }
          if ((!w->right || w->right->color == Color::Black) &&
              (!w->left || w->left->color == Color::Black)) {
            w->color = Color::Red;
            x = x_parent;
            x_parent = x_parent->parent;
          } else {
            if (!w->left || w->left->color == Color::Black) {
              w->right->color = Color::Black;
              w->color = Color::Red;
              rotate_left(w, root());
              w = x_parent->left;
            }
            w->color = x_parent->color;
            x_parent->color = Color::Black;
            if (w->left)
              w->left->color = Color::Black;
            rotate_right(x_parent, root());
            break;
          }
        }
      if (x)
        x->color = Color::Black;
    }

    delete Node::up_cast(y);
    --node_count;
  }
};

template <bool Const, class Val>
struct iterator {
  using value_type = std::conditional_t<Const, const Val, Val>;
  using reference = value_type&;
  using pointer = value_type*;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  NodeBase* node;

  iterator() {}
  constexpr explicit iterator(NodeBase* x) : node(x) {}

  iterator(const iterator&) = default;
  iterator& operator=(const iterator&) = default;

  constexpr iterator(const iterator<false, Val>& it)
  requires Const
      : node(it.node) {}

  reference operator*() const {
    return Node<Val>::up_cast(node)->val;
  }

  pointer operator->() const {
    return &Node<Val>::up_cast(node)->val;
  }

  constexpr iterator& operator++() {
    if (node->right) {
      node = node->right;
      while (node->left)
        node = node->left;
    } else {
      NodeBase* y = node->parent;
      while (node == y->right) {
        node = y;
        y = y->parent;
      }
      if (node->right != y)
        node = y;
    }

    return *this;
  }

  constexpr iterator operator++(int) {
    iterator tmp(node);
    ++*this;
    return tmp;
  }

  constexpr iterator& operator--() {
    if (node->color == Color::Red && node->parent->parent == node)
      node = node->right;
    else if (node->left) {
      NodeBase* y = node->left;
      while (y->right)
        y = y->right;
      node = y;
    } else {
      NodeBase* y = node->parent;
      while (node == y->left) {
        node = y;
        y = y->parent;
      }
      node = y;
    }
    return *this;
  }

  constexpr iterator operator--(int) {
    iterator tmp(node);
    --*this;
    return tmp;
  }

  bool operator==(const iterator& y) const = default;
};
} // namespace

template <class Key, class Val, class Hasher, class Compare, bool UniqueKeys>
class RbTree {
  using NodeBase = ::NodeBase;
  using Node = ::Node<Val>;
  using Header = ::Header<Val>;

  Header header;
  Compare key_compare;

  NodeBase* begin_root() const {
    return header.root();
  }

  NodeBase* end_root() const {
    return const_cast<NodeBase*>(&header.super_root);
  }

  static const Key& key(const NodeBase* node) {
    return Hasher()(Node::up_cast(node)->val);
  }

public:
  using iterator = ::iterator<false, Val>;
  using const_iterator = ::iterator<true, Val>;
  template <class Self>
  using cc_iterator =
      std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;

private:
  std::pair<NodeBase*, NodeBase*> get_insert_pos(const Key& k) {
    NodeBase* x = begin_root();
    NodeBase* y = end_root();

    bool comp = true;
    while (x) {
      y = x;
      comp = key_compare(k, key(x));
      x = comp ? x->left : x->right;
    }

    if constexpr (!UniqueKeys) {
      iterator j = iterator(y);
      if (comp) {
        if (j == begin())
          return {x, y};
        else
          --j;
      }
      if (key_compare(key(j.node), k))
        return {x, y};
      return {j.node, nullptr};
    } else
      return {x, y};
  }

  constexpr auto cmp(const Key& lhs, const Key& rhs) {
    if constexpr (UniqueKeys)
      return key_compare(lhs, rhs);
    else
      return !key_compare(rhs, lhs);
  };

  std::pair<NodeBase*, NodeBase*> get_insert_hint_pos(const_iterator position,
                                                      const Key& k) {
    if (position.node == end_root()) {
      if (size() > 0 && cmp(key(header.rightmost()), k))
        return {nullptr, header.rightmost()};
      else
        return get_insert_pos(k);
    } else if (cmp(k, key(position.node))) {
      iterator before(position.node);
      if (position.node == header.leftmost())
        return {header.leftmost(), header.leftmost()};
      else if (cmp(key((--before).node), k)) {
        if (!before.node->right)
          return {nullptr, before.node};
        else
          return {position.node, position.node};
      } else
        return get_insert_pos(k);
    } else {
      if constexpr (UniqueKeys)
        if (!key_compare(key(position.node), k))
          return {position.node, nullptr};

      iterator after(position.node);
      if (position.node == header.rightmost())
        return {nullptr, header.rightmost()};
      else if (cmp(k, key((++after).node))) {
        if (!position.node->right)
          return {nullptr, position.node};
        else
          return {after.node, after.node};
      } else {
        if constexpr (UniqueKeys)
          return get_insert_pos(k);
        else
          return {nullptr, nullptr};
      }
    }
  }

  iterator insert_node(NodeBase* x, NodeBase* p, Node* z) {
    bool insert_left = (x || p == end_root() || key_compare(key(z), key(p)));

    header.insert(insert_left, z, p);
    return iterator(z);
  }

  iterator insert_lower_node(NodeBase* p, Node* z) {
    bool insert_left = (p == end_root() || !key_compare(key(p), key(z)));

    NodeBase* base_z = z;
    header.insert(insert_left, base_z, p);
    return iterator(base_z);
  }

  iterator insert_equal_lower_node(Node* z) {
    NodeBase* x = begin_root();
    NodeBase* y = end_root();
    while (x) {
      y = x;
      x = !key_compare(key(x), key(z)) ? x->left : x->right;
    }
    return insert_lower_node(y, z);
  }

  NodeBase* lower_bound_base(NodeBase* x, NodeBase* y, const Key& k) const {
    while (x)
      if (!key_compare(key(x), k))
        y = x, x = x->left;
      else
        x = x->right;
    return y;
  }

  NodeBase* upper_bound_base(NodeBase* x, NodeBase* y, const Key& k) const {
    while (x)
      if (key_compare(k, key(x)))
        y = x, x = x->left;
      else
        x = x->right;
    return y;
  }

public:
  RbTree() = default;
  RbTree(const Compare& comp) : key_compare(comp) {}
  RbTree(const RbTree& x) = default;
  RbTree(RbTree&& x) = default;
  RbTree& operator=(const RbTree& x) = default;
  RbTree& operator=(RbTree&& x) = default;
  ~RbTree() = default;

  Compare key_comp() const {
    return key_compare;
  }

  auto begin(this auto&& self) {
    return cc_iterator<decltype(self)>(self.header.leftmost());
  }

  auto end(this auto&& self) {
    return cc_iterator<decltype(self)>(self.end_root());
  }

  std::size_t size() const {
    return header.node_count;
  }

  void swap(RbTree& t) {
    std::swap(*this, t);
  }

  template <class Arg>
  auto insert(Arg&& v) {
    auto res = get_insert_pos(Hasher()(v));

    if constexpr (UniqueKeys) {
      using Res = std::pair<iterator, bool>;
      if (res.second)
        return Res(
            insert_node(res.first, res.second, new Node(std::forward<Arg>(v))),
            true);
      return Res(iterator(res.first), false);
    } else
      return insert_node(res.first, res.second, new Node(std::forward<Arg>(v)));
  }

  template <class Arg>
  iterator insert_hint(const_iterator position, Arg&& v) {
    auto res = get_insert_hint_pos(position, Hasher()(v));
    if (res.second)
      return insert_node(res.first, res.second, new Node(std::forward<Arg>(v)));

    if constexpr (UniqueKeys)
      return iterator(res.first);
    else
      return insert_equal_lower_node(new Node(std::forward<Arg>(v)));
  }

  iterator erase(iterator position) {
    header.erase((position++).node);
    return position;
  }

  iterator erase(const_iterator position) {
    return erase(iterator(position.node));
  }

  void clear() {
    header.clear();
  }

  auto find(this auto&& self, const Key& k) {
    cc_iterator<decltype(self)> j(
        self.lower_bound_base(self.begin_root(), self.end_root(), k));
    return self.key_compare(k, key(j.node)) ? self.end() : j;
  }

  std::size_t count(const Key& k) const {
    std::pair<const_iterator, const_iterator> p = equal_range(k);
    return std::distance(p.first, p.second);
  }

  auto lower_bound(this auto&& self, const Key& k) {
    return cc_iterator<decltype(self)>(
        self.lower_bound_base(self.begin_root(), self.end_root(), k));
  }

  auto upper_bound(this auto&& self, const Key& k) {
    return cc_iterator<decltype(self)>(
        self.upper_bound_base(self.begin_root(), self.end_root(), k));
  }

  auto equal_range(this auto&& self, const Key& k) {
    using cc_iterator = cc_iterator<decltype(self)>;
    using Ret = std::pair<cc_iterator, cc_iterator>;

    NodeBase* x = self.begin_root();
    NodeBase* y = self.end_root();
    while (x) {
      if (self.key_compare(key(x), k))
        x = x->right;
      else if (self.key_compare(k, key(x)))
        y = x, x = x->left;
      else {
        NodeBase* xu(x);
        NodeBase* yu(y);
        y = x, x = x->left;
        xu = xu->right;
        return Ret(cc_iterator(self.lower_bound_base(x, y, k)),
                   cc_iterator(self.upper_bound_base(xu, yu, k)));
      }
    }
    return Ret(cc_iterator(y), cc_iterator(y));
  }

  friend bool operator==(const RbTree& x, const RbTree& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
  }

  friend auto operator<=>(const RbTree& x, const RbTree& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(),
                                                  y.end());
  }
};

#endif
