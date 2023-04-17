/**
 * @file BSTree.h
 * @brief The interface for an AVL binary search tree.
 * @date March 24, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Optional.h"

/// A binary search tree.
/// \tparam TKey
///   The type for keys in the tree.
/// \tparam TValue
///   The type for values in the tree.
template<typename TKey, typename TValue>
struct BSTree {
private:
  /// A tree node.
  struct Node {
    /// The parent of the node.
    Node *parent;
    
    /// The node that precedes this node (left child; less value).
    Node *prev;
    
    /// The node that follows this node (right child; greater value).
    Node *next;
    
    /// The associated key.
    TKey key;
    
    /// The associated value.
    TValue value;
    
    /// The node's balance factor.
    int balance;
    
    
    
    /// Create a node from a key-value pair.
    Node(const TKey &key, const TValue &value);
    
    /// Deep-copy a node.
    Node(const Node *other);
    
    /// Destroy the node.
    ~Node();
    
    /// Access either the `prev` or `next` value associated with `old` to
    /// replace it.
    /// \param[in] old
    ///   The child to replace.
    /// \returns
    ///   The location where the given child is stored in this node.
    Node *&replaceChild(Node *old);
    
    /// Rebalance the tree as necessary after an insertion operations,
    /// centered around this node.
    /// \param[inout] root
    ///   The root node of the tree.
    /// \returns
    ///   Whether or not the tree is now fully balanced.
    bool rebalance(Node *&root);
  };
  
  /// The root node of the tree.
  Node *root;
  
  
  
public:
  /// Create an empty binary search tree.
  BSTree();
  
  BSTree(const BSTree &other);
  
  BSTree &operator=(const BSTree &other);
  
  BSTree(BSTree &&other);
  
  BSTree &operator=(BSTree &&other);
  
  ~BSTree();
  
  
  
  /// Insert an item into the tree.
  /// \param[in] key
  ///   The key to insert.
  /// \param[in] value
  ///   The value associated with the given key.
  void insert(const TKey &key, const TValue &value);
  
  /// Access a value in the tree.
  /// \param[in] key
  ///   The key for which to lookup the associated value.
  /// \returns
  ///   The value associated with the given key, if the given key exists within
  ///   the tree or null otherwise.
  Optional<TValue> operator[](const TKey &key) const;
};

#include "BSTree.ipp"
