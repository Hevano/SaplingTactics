#pragma once

#include "TreeDesignNode.h"

#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>


using json = nlohmann::json;

namespace ArborMasterAdapter {
  class Adapter
  {
  public:

    std::shared_ptr<TreeDesignNode> getSubTree(json& data);

    std::shared_ptr<TreeDesignNode> loadTree(const std::string& path, std::string& debugPath);
  };
};

  