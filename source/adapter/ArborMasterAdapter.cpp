#include "ArborMasterAdapter.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace ArborMasterAdapter
{
TreeDesignNode Adapter::loadTree(std::string path)
{
  using json = nlohmann::json;
  std::fstream stream(path);
  json data = json::parse(stream);
  return TreeDesignNode();
}
}  // namespace ArborMasterAdapter