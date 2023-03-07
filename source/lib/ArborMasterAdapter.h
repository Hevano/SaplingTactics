#pragma once

#include "TreeDesignNode.h"

#include <nlohmann/json.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/containers/flat_map.hpp>

#include <string>
#include <fstream>
#include <iostream>


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

    return getSubTree(data["root"]);
  }
};

namespace ipc = boost::interprocess;

struct ActorUpdate {
  unsigned int nodeId;
  unsigned int actorId;
  unsigned int status;
};

class Debugger {
public:

  //Initiates IPC with the designer, returning false if it fails
  bool init()
  {
    ipc::message_queue::remove("NodeUpdateMessageQueue");
    
    return true;
  }

  ~Debugger()
  {

  }

  //Updates the running status of a node
  void updateDebugStatus(
    unsigned int nodeId,
    unsigned int actorId,
    unsigned int status)
  {
    ipc::message_queue  msgQueue(ipc::open_or_create, "NodeUpdateMessageQueue",
      100,
      sizeof(ActorUpdate)
    );
    ActorUpdate update(nodeId, actorId, status);
    try {
      if (msgQueue.get_num_msg() < msgQueue.get_max_msg()) {
        msgQueue.send(&update, sizeof(ActorUpdate), 0);
      }
    }
    catch (ipc::interprocess_exception& ex) {
      std::cout << "Error: " << ex.what() << std::endl;
    }
  };

  //Updates the blackboard value of an actor
  void updateDebugBlackboard(
    unsigned int actorId,
    std::string key,
    std::string value)
  {

  };

  //declares that there is an actor who uses a tree at the path
  void createDebugActor(unsigned actorId, std::string tree_path) 
  {
  
  };

  //declares that 
  void deleteDebugActor(unsigned actorId)
  {

  };

};
  