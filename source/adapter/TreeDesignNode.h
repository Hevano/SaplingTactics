#pragma once

#include <memory>
#include <vector>
#include <string>

struct TreeDesignNode
{
  unsigned int nodeId;
  std::string name;
  std::vector<std::shared_ptr<TreeDesignNode>> children;

  TreeDesignNode(const std::string& n, unsigned int id)
      : nodeId(id)
      , name(n)
  {};

  TreeDesignNode(const TreeDesignNode&) = default;


  // Potential further improvement, store parameters for constructors of
  // implementation nodes
};