
//
// Created by Nadia Hyder on 5/22/18.
//

#include <iostream>
#include <cassert>
#include "AVL.h"
#include "json.hpp"
#include <queue>

AVLNode::AVLNode(int key) : 	height(0), key_(key),
							   parent_(std::weak_ptr<AVLNode>()),
							   left_(nullptr),
							   right_(nullptr) {

}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
	height(0),
	key_(key),
	parent_(parent),
	left_(nullptr),
	right_(nullptr) {}

int AVLNode::getHeight() {
  return static_cast<int>(height);
}

int AVLNode::getBalance() {
  int rightHeight = -1;
  int leftHeight = -1;

  if(left_ != nullptr)
	leftHeight = static_cast<int>(left_->height);
  if(right_ != nullptr)
	rightHeight = static_cast<int>(right_->height);

  return rightHeight - leftHeight;
}

bool AVLNode::IsLeaf() const {
  return left_ == nullptr && right_ == nullptr;
}

AVLTree::AVLTree() : size_(0) {}

int AVLTree::getRoot() {
  return root_->key_;
}

bool AVLNode::hasLeftChild() const {
  return left_!= nullptr;
}

bool AVLNode::hasRightChild() const {
  return right_!= nullptr;
}


void AVLTree::Insert(int key) {
  if (root_ == nullptr) {
	root_ = std::make_shared<AVLNode>(key);
	size_++;
	return;
  }
  std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;

  while (currentNode != nullptr) {
	lastNode = currentNode; // FIX WHAT ARE WE DOING WITH LAST NODE
	currentNode = (key < currentNode->key_) ?
				  currentNode->left_ : currentNode->right_;
  }

  std::shared_ptr<AVLNode> insertedNode = std::make_shared<AVLNode>(key, lastNode);
  if (key < lastNode->key_) {
	lastNode->left_ = insertedNode;
	size_++;
  } else {
	lastNode->right_ = insertedNode;
	size_++;
  }

   // updateHeight, increment size

  // update height and re-balance from lastNode up
  // problem is with updating heights
  while(lastNode != nullptr) {
	updateHeight(lastNode);
	rebalanceForInsert(lastNode, key);
	lastNode = lastNode->parent_.lock();
  }
}

/* note: conditions for rebalancing are different for deletion */
void AVLTree::rebalanceForInsert(std::shared_ptr<AVLNode> node, int key) {

  int balance = node->getBalance();

  // Left Left Case
  if(node->hasLeftChild()) {
	if (balance < -1 && key < node->left_->key_) {
	  //std::cout << "left left case for node " << node->key_ << std::endl;
	  rightRotate(node);
	}
  }

  // Left Right Case
  // if (balance > 1 && key > node->left_->key_)
  if(node->hasLeftChild()) {
	if (balance < -1 && key > node->left_->key_) {
	  //std::cout << "left right case " << std::endl;
	  leftRotate(node->left_);
	  rightRotate(node);
	}
  }


  if(node->hasRightChild()) {
	// Right Right Case
	if (balance > 1 && key > node->right_->key_) {
	  //std::cout << "right right case " << std::endl;
	  leftRotate(node);
	}
  }

  if(node->hasRightChild()) {
	// Right Left Case
	if (balance > 1 && key < node->right_->key_) {
	  rightRotate(node->right_);
	  leftRotate(node);
	}
  }
}

void AVLTree::Delete(int key) {
  std::shared_ptr<AVLNode> currentNode = root_;
  std::shared_ptr<AVLNode> parent;

  while (currentNode != nullptr) {
    parent = currentNode->parent_.lock();

	if (currentNode->key_ == key) {
	  if (currentNode->IsLeaf()) {
		//std::cout << "is leaf delete case" << std::endl;
		DeleteLeaf(currentNode);
		size_--; //assert(size_ >= 0);
	  } else if (currentNode->left_ == nullptr) {
		//std::cout << "left null ptr delete case" << std::endl;
		assert(currentNode->right_ != nullptr);


		if(currentNode != root_) {
		  if(currentNode == parent->right_) { // replace right child
		    parent->right_ = currentNode->right_;
		    currentNode->right_->parent_= parent;
		  }
		  else { // replace left child
			parent->left_ = currentNode->right_;
			currentNode->right_->parent_= parent;
		  }

		}

		else {
		  root_ = currentNode->right_;
		}

		currentNode = currentNode->right_; // for traversal purposes?
		size_--; //assert(size_ >= 0);
	  } else if (currentNode->right_ == nullptr) {
	    //std::cout << "deleting node" << currentNode->key_ << " w null right child" << std::endl;

		size_--; //assert(size_ >= 0);

		//std::cout << "right nullptr delete case" << std::endl;
		assert(currentNode->left_ != nullptr);

		if(currentNode != root_) {
		  if(currentNode == parent->right_) { // replace right child
			parent->right_ = currentNode->left_;
			currentNode->left_->parent_= parent;
		  }
		  else { // replace left child
			parent->left_ = currentNode->left_;
			currentNode->left_->parent_= parent;
		  }

		}

		else {
		  root_ = currentNode->left_;
		}
	  } else {
	    //std::cout << "here b4 delete min" << std::endl;
	    if(currentNode->hasRightChild())
			currentNode->key_ = BSTDeleteMin(currentNode->right_); // delete's right node's minimum node. currentNode is in position of deletion
		size_--; //assert(size_ >= 0);
	  }
	  break;
	}
	currentNode = (key < currentNode->key_) ?
				  currentNode->left_ : currentNode->right_; // traverse through tree
  }

	while(currentNode != nullptr) {
	  updateHeight(currentNode);
	  if (currentNode != nullptr) {
		rebalanceForDelete(currentNode);
	  } else {
		if (parent != nullptr) {
		  rebalanceForDelete(parent);
		}
	  }
	  currentNode = currentNode->parent_.lock();
	}
}

void AVLTree::DeleteLeaf(std::shared_ptr<AVLNode> currentNode) {
  std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
  if (parent == nullptr) {
	// Delete root
	root_ = nullptr;
	//size_--; //assert(size_ == 0);
  } else {
	if (parent->right_ == currentNode) {
	  parent->right_ = nullptr;
	} else if (parent->left_ == currentNode) {
	  parent->left_ = nullptr;
	} else {
	  std::cerr << "AVL::DeleteLeaf Error: inconsistent state\n";
	}
	//size_--; assert(size_ >= 0);
  }

  if(parent != nullptr) {
	updateHeight(parent);
  }
}

/* note: conditions for rebalancing are different for deletion */
void AVLTree::rebalanceForDelete(std::shared_ptr<AVLNode> node) {
  int balance = node->getBalance();
  // Left Left Case
  if(node->hasLeftChild()) {
	if (balance < -1 && node->left_->getBalance() <= 0) {
	  //std::cout << "left left case for node " << node->key_ << std::endl;
	  rightRotate(node);
	  return;
	}
  }

  // Left Right Case
  if(node->hasLeftChild()) {
	if (balance < -1 && node->left_->getBalance() > 0) {
	  //std::cout << "left right case " << std::endl;
	  leftRotate(node->left_);
	  rightRotate(node);
	  return;
	}
  }


  if(node->hasRightChild()) {
	// Right Right Case
	if (balance > 1 && node->right_->getBalance() >= 0) {
	 // std::cout << "right right case " << std::endl;
	  leftRotate(node);
	  return;
	}
  }

  if(node->hasRightChild()) {
	// Right Left Case
	if (balance > 1 && node->right_->getBalance() < 0) {
	 // std::cout << "right left case " << std::endl;
	  rightRotate(node->right_);
	  leftRotate(node);
	  return;
	}
  }
}

// fixme size is not changing
int AVLTree::DeleteMin() {
  //std::cout << "inside delete min" << std::endl;

  int val = -1;
  if(root_ != nullptr) {
	val = BSTDeleteMin(root_);
	size_--; //assert(size_ >= 0);
  }

  if(val == -1) {
    std::cout << "val is -1" << std::endl;
  }
  return val;
}


// note: modified by Nadia
int AVLTree::BSTDeleteMin(std::shared_ptr<AVLNode> currentNode) {

  std::shared_ptr<AVLNode> lastNode = nullptr;
  while (currentNode != nullptr) {
	lastNode = currentNode;
	currentNode = currentNode->left_;
  } // new value to look at is lastNode

  int result = lastNode->key_;


  std::shared_ptr<AVLNode> parent = lastNode->parent_.lock(); // parent = 45

  if (parent == nullptr) {
	// lastNode is root
	if (lastNode->hasRightChild()) {
	  root_ = lastNode->right_;
	  lastNode->right_->parent_.reset();
	} else {
	  root_ = nullptr;
	}
  } else {
	if(parent->hasLeftChild()) {
	  if (lastNode == parent->left_) { // modified here

	    if(lastNode->hasRightChild()) {
		  parent->left_ = lastNode->right_;
		  lastNode->right_->parent_ = parent;
	    }
		else {
	      parent->left_ = nullptr;
	    }
	  }
	}
	if(parent->hasRightChild()) {
	  if (lastNode == parent->right_) {

		if(lastNode->hasRightChild()) {
		  parent->right_ = lastNode->right_;
		  lastNode->right_->parent_ = parent;
		}
		else {
		  parent->right_ = nullptr;
		}
	  }
	}
  }



  upwardHeightUpdate(lastNode, nullptr);

  while(lastNode != nullptr) {
	if (lastNode!= nullptr) {
	  rebalanceForDelete(lastNode);
	} else {
	  if (parent != nullptr) {
		rebalanceForDelete(parent);
	  }
	}
	lastNode = lastNode->parent_.lock();
  }

  //size_--;
  return result;
}

void AVLTree::rightRotate(std::shared_ptr<AVLNode> node) {

  std::shared_ptr<AVLNode> leftNode = node->left_; // leftnode is x
  leftNode->parent_ = node->parent_; // this will be nullptr if root

  std::shared_ptr<AVLNode> leftRightNode = nullptr; // leftRightNode is T2
  if(leftNode != nullptr) {
	leftRightNode = leftNode->right_;
	leftNode->right_ = node;
  }


  if(node != root_) // otherwise can not access node->parent
  {
	if(node == node->parent_.lock()->right_) {
	  node->parent_.lock()->right_ = leftNode;
	}
	else {
	  node->parent_.lock()->left_ = leftNode;
	  leftNode->parent_ = node->parent_;
	}
  }
  else {
    root_ = leftNode;
  }

  if(leftNode != nullptr) {
	node->parent_ = leftNode;
  }
  node->left_ = leftRightNode;

  if(leftRightNode != nullptr) {
	leftRightNode->parent_ = node;
  }

  upwardHeightUpdate(node, nullptr);
}

/* updates the heights of every node in the tree up from node, performed after rebalance */
void AVLTree::upwardHeightUpdate(std::shared_ptr<AVLNode> node, std::shared_ptr<AVLNode> endNode) {

  int rightHeight = -1;
  int leftHeight = -1;
  if(node->hasLeftChild()) {
	updateHeight(node->left_);
	leftHeight = static_cast<int>(node->left_->height);
  }

  if(node->hasRightChild()) {
	updateHeight(node->right_);
	rightHeight = static_cast<int>(node->right_->height);
  }


  node->height = 1 + std::max(rightHeight, leftHeight);

  node = node->parent_.lock();
  while(node != endNode) {
	updateHeight(node);
	node = node->parent_.lock();
  }
}

void AVLTree::leftRotate(std::shared_ptr<AVLNode> node) {
  //std::cout << "node is " << node->key_ << std::endl;
  std::shared_ptr<AVLNode> rightNode = node->right_;
  rightNode->parent_ = node->parent_;

 std::shared_ptr<AVLNode> rightLeftNode = nullptr;
  if(rightNode != nullptr) {
  	rightLeftNode = rightNode->left_;
  	rightNode->left_ = node;
  }

  if(node != root_) {
	if (node == node->parent_.lock()->right_) {
	  node->parent_.lock()->right_ = rightNode;

	} else {
	  node->parent_.lock()->left_ = rightNode;
	  rightNode->parent_ = node->parent_;
	}
  }
  else {
    root_ = rightNode;
  }

  if(rightNode != nullptr) {
    node->parent_ = std::weak_ptr<AVLNode>(node->right_);
  }

  node->right_ = rightLeftNode;

  if(rightLeftNode != nullptr)
  	rightLeftNode->parent_ = node;

  upwardHeightUpdate(node, nullptr);

}

// updates height of a single node
void AVLTree::updateHeight(std::shared_ptr<AVLNode> curNode) {

  int rightHeight = -1;
  int leftHeight = -1;

	if(curNode->left_ != nullptr)
	  leftHeight = static_cast<int>(curNode->left_->height);
	if(curNode->right_ != nullptr)
	  rightHeight = static_cast<int>(curNode->right_->height);

	curNode->height = 1 + std::max(rightHeight, leftHeight);

}

void AVLTree::printTree(std::shared_ptr<AVLNode> root) {
  printTreeHelper(root, 0);
}

void AVLTree::printTreeHelper(std::shared_ptr<AVLNode> root, int space) {
  const int COUNT = 10;

  // Base case
  if (root == nullptr)
	return;

  // Increase distance between levels
  space += COUNT;

  // Process right child first
  printTreeHelper(root->right_, space);

  // Print current node after space
  // count
  std::cout << std::endl;
  for (int i = COUNT; i < space; ++i) {
	std::cout << " ";
  }
  std::cout << root->key_ << std::endl;

  // Process left child
  printTreeHelper(root->left_, space);
}


std::string AVLTree::JSON() const {
  nlohmann::json result;
  std::queue< std::shared_ptr<AVLNode> > nodes;
  if (root_ != nullptr) {
	result["root"] = root_->key_;
	nodes.push(root_);
	while (!nodes.empty()) {
	  auto v = nodes.front();
	  nodes.pop();
	  std::string key = std::to_string(v->key_);
	  result[key]["balance factor"] = v->getBalance();
	  result[key]["height"] = v->height;
	  if (v->left_ != nullptr) {
		result[key]["left"] = v->left_->key_;
		nodes.push(v->left_);
	  }
	  if (v->right_ != nullptr) {
		result[key]["right"] = v->right_->key_;
		nodes.push(v->right_);
	  }
	  if (v->parent_.lock() != nullptr) {
		result[key]["parent"] = v->parent_.lock()->key_;
	  } else {
		result[key]["root"] = true;
	  }
	}

	result["height"] = root_->height;
	result["size"] = size_;
  } else {
	result["height"] = 0;
	result["size"] = 0;
  }
  return result.dump(2) + "\n";
}
