#pragma once

#include "TreeDesignNode.h"

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class Adapter
{
public:

  std::shared_ptr<TreeDesignNode> getSubTree(json& data)
  {
    int id = data["id"].get<int>();
    auto name = data["name"].get<std::string>();
    std::shared_ptr<TreeDesignNode> root = std::make_shared<TreeDesignNode>(name, id);


    auto dump = data.dump();

    for (auto childJson : data["children"]) {
      root->children.push_back(getSubTree(childJson));
    }

    return root;
  }

  std::shared_ptr<TreeDesignNode> loadTree(const std::string& path)
  {
    std::ifstream stream(path);
    json data;
    stream >> data;

    auto is_obj = data.dump();

    return getSubTree(data["root"]);
  }

private:
  void updateDebugStatus(unsigned int nodeId,
                         unsigned int actorId,
                         unsigned int status);

  void updateDebugBlackboard(unsigned int actorId,
                             std::string key,
                             std::string value);

  void createDebugActor(unsigned actorId, std::string tree_path);
};
  