//
// Created by Roy Tsui on 5/26/2018.
//

#include <fstream>
#include <iostream>
#include "json.hpp"
#include "AVL.h"

AVLTree readAVLTree(nlohmann::json AVLOperationsJSON);

int main(int argc, char** argv) {
  std::ifstream file;

  file.open(argv[1]);

  nlohmann::json AVLOperationsJSON;
  if(file.is_open()) {
    file >> AVLOperationsJSON;
  }

  AVLTree AVL = readAVLTree(AVLOperationsJSON);

  std::cout << std::setw(4) <<  AVL.JSON() << std::endl;

  std::cout << "FINAL AVL TREE:" << std::endl;
  AVL.printTree(AVL.root_);
}

AVLTree readAVLTree(nlohmann::json AVLOperationsJSON) {
  int numOperations = AVLOperationsJSON["metadata"]["numOps"];
  AVLTree AVL;
  int AVLNodeKey = 0;

  // will loop through all the samples, stopping when it reaches the "metadata" key
  int i = 0;
  for(auto itr = AVLOperationsJSON.begin(); i < numOperations || (itr != AVLOperationsJSON.end() && itr.key() != "metadata"); ++itr) {
    if (AVLOperationsJSON[itr.key()]["operation"] == "Insert") {
      AVLNodeKey = AVLOperationsJSON[itr.key()]["key"];
	  AVL.Insert(AVLNodeKey);
    }
	else if (AVLOperationsJSON[itr.key()]["operation"] == "Delete") {
	  AVLNodeKey = AVLOperationsJSON[itr.key()]["key"];
	  AVL.Delete(AVLNodeKey);
	}
	else if (AVLOperationsJSON[itr.key()]["operation"] == "DeleteMin") {
	  AVL.DeleteMin();
	}
	++i;
  }

  return AVL;
}