#pragma once

#include <memory>
#include <vector>

namespace ArborMasterAdapter
{
struct TreeDesignNode
{
  unsigned int nodeId;
  std::vector<std::shared_ptr<TreeDesignNode>> children;
  //Potential further improvement, store parameters for constructors of implementation nodes
};
}