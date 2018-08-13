//
// Created by Nadia Hyder on 5/22/18.
//

#include "BST.h"
#ifndef PROGRAM3_AVL_H
#define PROGRAM3_AVL_H

class AVLTree;

class AVLNode {
 public:
  AVLNode(int key);
  AVLNode(int key, std::weak_ptr<AVLNode> parent);
  int getHeight();
  int getBalance();
  void updateHeight();
  bool IsLeaf() const;
 //private:
  size_t height;
  int key_;
  std::weak_ptr<AVLNode> parent_;
  std::shared_ptr<AVLNode> left_;
  std::shared_ptr<AVLNode> right_;
  bool hasLeftChild() const;
  bool hasRightChild() const;
  void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> newNode);

  friend AVLTree; // make AVLTree a friend of AVLNode
};

class AVLTree {
 public:
  AVLTree();
  void Insert(int key); // override;
  void Delete(int key); // override;
  int DeleteMin(); // override;
  void rebalanceForInsert(std::shared_ptr<AVLNode> node, int key);
  void rebalanceForDelete(std::shared_ptr<AVLNode> node);
  void rightRotate(std::shared_ptr<AVLNode> node);
  void leftRotate(std::shared_ptr<AVLNode> node);
  void printTree(std::shared_ptr<AVLNode> root);
  void printTreeHelper(std::shared_ptr<AVLNode> root, int space);
  int getRoot();
  std::string JSON() const;

 //private:
  void DeleteLeaf(std::shared_ptr<AVLNode> currentNode);
  int BSTDeleteMin(std::shared_ptr<AVLNode> currentNode);
  void updateHeight(std::shared_ptr<AVLNode> curNode);
  void upwardHeightUpdate(std::shared_ptr<AVLNode> curNode, std::shared_ptr<AVLNode> endNode);

  std::shared_ptr<AVLNode> root_;
  size_t size_;

};

#endif //PROGRAM3_AVL_H
