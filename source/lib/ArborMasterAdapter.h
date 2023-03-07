#pragma once

#include "TreeDesignNode.h"

#include <nlohmann/json.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/flat_map.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>


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

//struct shm_remove
//{
//  shm_remove() { shared_memory_object::remove("MySharedMemory"); }
//  ~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
//} remover;

class Debugger {
private:
    //Max number of bytes our shared memory buffer can hold
    const int MAX_BYTES = 65536;

    typedef ipc::allocator<std::pair<const std::string, std::string>, ipc::managed_shared_memory::segment_manager> bb_allocator_type;
    typedef ipc::flat_map<std::string, std::string, std::less<std::string>, bb_allocator_type> bb_map_type;

    typedef ipc::allocator<std::pair<unsigned int, std::string>, ipc::managed_shared_memory::segment_manager> actorid_allocator_type;
    typedef ipc::flat_map<unsigned int, std::string, std::less<unsigned int>, bb_allocator_type> actorid_map_type;

    ipc::managed_shared_memory m_segment;
    std::unique_ptr<bb_map_type> m_blackBoardMap;
    std::unique_ptr<actorid_map_type> m_actorIdMap;

    unsigned int m_currentActorId;

public:

  //Initiates IPC with the designer, returning false if it fails
  bool init()
  {
    //clears existing message queues
    ipc::message_queue::remove("NodeUpdateMessageQueue");
    ipc::message_queue::remove("ActorSelectMessageQueue");

    //create segment and the flat maps inside of it
    m_segment = ipc::managed_shared_memory (ipc::create_only, "DebuggerSharedMemory", 65536);
    m_blackBoardMap.reset(m_segment.find_or_construct<bb_map_type>("BlackboardMap")(m_segment.get_segment_manager()));
    m_actorIdMap.reset(m_segment.find_or_construct<actorid_map_type>("ActorIdMap")(m_segment.get_segment_manager()));
    
    return true;
  }

  //returns true if actor has changed, in which case blackboard should be updated
  bool tick() {
    size_t recv_size;
    unsigned int priority;
    unsigned int *id = nullptr;

    ipc::message_queue mq(ipc::open_or_create, "ActorSelectMessageQueue", 100, sizeof(unsigned int));
    try {
      mq.receive(id, sizeof(unsigned int), recv_size, priority);
      m_currentActorId = *id;
      return true;
    }
    catch (ipc::interprocess_exception& ex) {
      //throws error if queue is empty
    }
    return false;
  }

  unsigned int getCurrentActorId() const {
    return m_currentActorId;
  }

  ~Debugger()
  {
    // Deallocate the object in the shared memory segment
    m_segment.destroy<actorid_map_type>("ActorIdMap");
    m_segment.destroy<bb_map_type>("BlackboardMap");

    // Deallocate the shared memory segment
    ipc::shared_memory_object::remove("DebuggerSharedMemory");
  }

  //Updates the running status of a node
  void updateDebugStatus(
    unsigned int nodeId,
    unsigned int actorId,
    unsigned int status)
  {
    //Only update the message queue if we are currently watching that actor
    if (actorId != m_currentActorId) return;
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
      m_blackBoardMap->insert({ "key1", "value1" });
      m_blackBoardMap->insert({ "key2", "value2" });
  };

  //declares that there is an actor who uses a tree at the path
  void createDebugActor(unsigned int actorId, std::string treePath) 
  {
    m_actorIdMap->insert({ actorId, treePath });
  };

};
  