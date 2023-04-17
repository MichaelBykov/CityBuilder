/**
 * @file BSTree.cpp
 * @brief Implement the interface for an AVL binary search tree.
 * @date March 24, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "BSTree.h"

/* -------------------------------------------------------------------------- *\
|                                                                              |
| Node constructors and destructors                                            |
|                                                                              |
\* -------------------------------------------------------------------------- */

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::Node::Node(const TKey &key, const TValue &value) :
  key(key), value(value), next(nullptr), prev(nullptr), parent(nullptr),
  balance(0) { }

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::Node::Node(const Node *other) :
  key(other->key), value(other->value), balance(other->balance),
  parent(nullptr),
  next(other->next ? new Node(other->next) : nullptr),
  prev(other->prev ? new Node(other->prev) : nullptr) { }

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::Node::~Node() {
  if (next != nullptr) {
    delete next;
    next = nullptr;
  }
  if (prev != nullptr) {
    delete prev;
    prev = nullptr;
  }
}



/* -------------------------------------------------------------------------- *\
|                                                                              |
| Node methods                                                                 |
|                                                                              |
\* -------------------------------------------------------------------------- */

template<typename TKey, typename TValue>
typename BSTree<TKey, TValue>::Node *&BSTree<TKey, TValue>::Node::replaceChild(Node *old) {
  if (prev == old)
    return prev;
  else
    return next;
}

template<typename TKey, typename TValue>
bool BSTree<TKey, TValue>::Node::rebalance(Node *&root) {
  // Determine what kind of rebalancing needs to happen
  switch (balance) {
  case 0:
    // The node is balanced
    return true;
  
  case  1:
  case -1:
    // The node is not yet unbalanced
    return false;
  
  case -2:
    // The left side needs to be rebalanced
    if (prev->balance <= 0) {
      // The node is rebalanced as follows:
      //     a        b
      //    / \      / \
      //   b   3 -> 1   a
      //  / \       |  / \
      // 1+1 2     +1 2   3
      
      // Get the new root
      Node *node = prev;
      
      // Update the parents
      node->parent = parent;
      (parent ? parent->replaceChild(this) : root) = node;
      parent = node;
      
      if (node->next != nullptr)
        node->next->parent = this;
      
      // Update the balance
      node->balance = 0;
      balance = 0;
      
      // Move the nodes
      prev = node->next;
      node->next = this;
    } else {
      // The node is rebalanced as follows:
      //     a            1
      //    / \         /   \
      //   b   3 ->   b       a
      //  / \        / \     / \
      // 2   1+1    2  +1L +1R  3
      
      // Get the new root
      Node *node = prev->next;
      
      // Update the parents
      node->parent = parent;
      (parent ? parent->replaceChild(this) : root) = node;
      prev->parent = node;
      parent = node;
      
      if (node->prev != nullptr)
        node->prev->parent = prev;
      if (node->next != nullptr)
        node->next->parent = this;
      
      // Update the balance
      if (node->balance < 0) {
        balance = -1;
        prev->balance = 0;
      } else if (node->balance == 0) {
        balance = 0;
        prev->balance = 0;
      } else {
        balance = 0;
        prev->balance = 1;
      }
      node->balance = 0;
      
      // Move the nodes
      prev->next = node->prev;
      node->prev = prev;
      
      prev = node->next;
      node->next = this;
    }
    return true;
  
  case 2:
    // The right side needs to be rebalanced
    if (next->balance >= 0) {
      // The node is rebalanced as follows:
      //     a              b
      //    / \            / \
      //   3   b     ->   a   1
      //      / \        / \  |
      //     2   1+1    3   2 +1
      
      // Get the new root
      Node *node = next;
      
      // Update the parents
      node->parent = parent;
      (parent ? parent->replaceChild(this) : root) = node;
      parent = node;
      
      if (node->prev != nullptr)
        node->prev->parent = this;
      
      // Update the balance
      node->balance = 0;
      balance = 0;
      
      // Move the nodes
      next = node->prev;
      node->prev = this;
    } else {
      // The node is rebalanced as follows:
      //     a              1
      //    / \           /   \
      //   3   b   ->   a       b
      //      / \      / \     / \
      //     1+1 2    3  +1L +1R  2
      
      // Get the new root
      Node *node = next->prev;
      
      // Update the parents
      node->parent = parent;
      (parent ? parent->replaceChild(this) : root) = node;
      next->parent = node;
      parent = node;
      
      if (node->next != nullptr)
        node->next->parent = next;
      if (node->prev != nullptr)
        node->prev->parent = this;
      
      // Update the balance
      if (node->balance > 0) {
        balance = 1;
        next->balance = 0;
      } else if (node->balance == 0) {
        balance = 0;
        next->balance = 0;
      } else {
        balance = 0;
        next->balance = -1;
      }
      node->balance = 0;
      
      // Move the nodes
      next->prev = node->next;
      node->next = next;
      
      next = node->prev;
      node->prev = this;
    }
    return true;
  }
  return true;
}



/* -------------------------------------------------------------------------- *\
|                                                                              |
| Tree constructors and destructors                                            |
|                                                                              |
\* -------------------------------------------------------------------------- */

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::BSTree() : root(nullptr) { }

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::BSTree(const BSTree &other) :
  root(other.root ? new Node(other.root) : nullptr) { }

template<typename TKey, typename TValue>
BSTree<TKey, TValue> &BSTree<TKey, TValue>::operator=(const BSTree &other) {
  if (root != nullptr)
    delete root;
  root = other.root ? new Node(other.root) : nullptr;
}

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::BSTree(BSTree &&other) : root(other.root) {
  other.root = nullptr;
}

template<typename TKey, typename TValue>
BSTree<TKey, TValue> &BSTree<TKey, TValue>::operator=(BSTree &&other) {
  if (root != nullptr)
    delete root;
  root = other.root;
  other.root = nullptr;
}

template<typename TKey, typename TValue>
BSTree<TKey, TValue>::~BSTree() {
  if (root != nullptr) {
    delete root;
    root = nullptr;
  }
}



/* -------------------------------------------------------------------------- *\
|                                                                              |
| Tree methods                                                                 |
|                                                                              |
\* -------------------------------------------------------------------------- */

template<typename TKey, typename TValue>
void BSTree<TKey, TValue>::insert(const TKey &key, const TValue &value) {
  // Create the node to insert
  Node *node = new Node(key, value);
  
  // Set the root node if it doesn't exist yet
  if (root == nullptr) {
    root = node;
    return;
  }
  
  // Find the appropriate place for the node
  {
    // Go down the tree to the most applicable space
    Node *parent = root;
    bool left;
    while (true) {
      // Determine which side to examine
      Node *&child = key < parent->key ? parent->prev : parent->next;
      
      if (child == nullptr) {
        // No where further to go: insert the node
        child = node;
        break;
      } else
        parent = child;
    }
    
    // Set the parent
    node->parent = parent;
  }
  
  // Rebalance the tree
  {
    // Go up the tree propagating the imbalance
    Node *last = node;
    Node *parent = last->parent;
    int change = 0;
    while (parent != nullptr) {
      if (parent->prev == last)
        // Inserted node is on the left side
        parent->balance--;
      else
        // Inserted node is on the right side
        parent->balance++;
        
      // Rebalance the node as necessary
      if (parent->rebalance(root))
        // The tree has been balanced
        return;
      
      last = parent;
      parent = parent->parent;
    }
  }
}

template<typename TKey, typename TValue>
Optional<TValue> BSTree<TKey, TValue>::operator[](const TKey &key) const {
  if (root == nullptr)
    // Empty tree, nothing to access
    return nullptr;
  
  Node *node = root;
  while (node != nullptr) {
    if (node->key == key)
      return node->value;
    else if (key < node->key)
      node = node->prev;
    else
      node = node->next;
  }
  
  // Nothing matched
  return nullptr;
}
