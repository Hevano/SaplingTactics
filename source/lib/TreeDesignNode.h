#pragma once

#include <memory>
#include <vector>

struct TreeDesignNode
{
  unsigned int nodeId;
  std::vector<std::shared_ptr<TreeDesignNode>> children;

  TreeDesignNode(unsigned int id)
      : nodeId(id) {
    int x = 5;
  };


  // Potential further improvement, store parameters for constructors of
  // implementation nodes
};