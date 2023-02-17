#pragma once

#include "TreeDesignNode.h"

#include <string>

namespace ArborMasterAdapter
{
class Adapter
{
  void updateDebugStatus(unsigned int nodeId,
                         unsigned int actorId,
                         unsigned int status);

  void updateDebugBlackboard(unsigned int actorId,
                             std::string key,
                             std::string value);

  void createDebugActor(unsigned actorId, std::string tree_path);

  TreeDesignNode loadTree(std::string path);
};
}
  