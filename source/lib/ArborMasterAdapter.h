#pragma once

#include "TreeDesignNode.h"

#include <nlohmann/json.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

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

  std::shared_ptr<TreeDesignNode> loadTree(const std::string& path, std::string& debugPath)
  {
    std::ifstream stream(path);
    json data;
    stream >> data;

    debugPath = data["debugPath"].get<std::string>();

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

    typedef ipc::allocator<char, ipc::managed_shared_memory::segment_manager> char_allocator;
    typedef ipc::basic_string<char, std::char_traits<char>, char_allocator> char_string;

    typedef ipc::allocator<std::pair<const unsigned int, char_string>, ipc::managed_shared_memory::segment_manager> actorid_allocator;
    typedef ipc::flat_map<unsigned int, char_string, std::less<unsigned int>, actorid_allocator> actorid_map_type;

    typedef ipc::allocator<std::pair<const ipc::basic_string<char>, ipc::basic_string<char>>, ipc::managed_shared_memory::segment_manager> bb_allocator_type;
    typedef ipc::flat_map<ipc::basic_string<char>, ipc::basic_string<char>, std::less<ipc::basic_string<char>>, bb_allocator_type> bb_map_type;

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
    // Deallocate the shared memory segment, if it already exists
    ipc::shared_memory_object::remove("DebuggerSharedMemory");

    //create segment and the flat maps inside of it
    m_segment = ipc::managed_shared_memory (ipc::create_only, "DebuggerSharedMemory", 65536);
    m_blackBoardMap.reset(m_segment.find_or_construct<bb_map_type>("BlackboardMap")(m_segment.get_segment_manager()));
    m_actorIdMap.reset(m_segment.find_or_construct<actorid_map_type>("ActorIdMap")(m_segment.get_segment_manager()));
 
    return true;
  }

  //returns true if actor has changed, in which case blackboard should be updated
  bool tick() {
    ipc::message_queue::size_type recv_size;
    unsigned int priority;
    unsigned int id = 0;

    ipc::message_queue mq(ipc::open_or_create, "ActorSelectMessageQueue", 100, sizeof(unsigned int));
    try {
      if (mq.get_num_msg() > 0) {
        mq.receive(&id, sizeof(id), recv_size, priority);
        if (recv_size == sizeof(id)) {
          m_currentActorId = id;
          return true;
        }
      }
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

    //Release the flat map pointers rather than allow unique_ptr to free them. TODO: add custom deleter to handle this properly
    m_actorIdMap.release();
    m_blackBoardMap.release();

    // Deallocate the shared memory segment
    ipc::shared_memory_object::remove("DebuggerSharedMemory");
  }

  //Updates the running status of a node
  void updateNodeStatus(
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
    if (key.empty()) {
      return;
    }
    else if (value.empty()) {
      m_blackBoardMap->erase(ipc::basic_string<char>(key.c_str()));
    }
    else {
      auto keyBoostString = ipc::basic_string<char>(key.c_str());
      if (m_blackBoardMap->contains(keyBoostString))
      {
        m_blackBoardMap->at(keyBoostString) = ipc::basic_string<char>(value.c_str());
      }
      else {
        m_blackBoardMap->insert({ keyBoostString, ipc::basic_string<char>(value.c_str()) });
      }
      
    }
  };

  void updateDebugBlackboard(
    unsigned int actorId,
    std::pair<std::string, std::string> keyValue)
  {
    updateDebugBlackboard(actorId, keyValue.first, keyValue.second);
  };

  //declares that there is an actor who uses a tree at the path
  void createDebugActor(unsigned int actorId, std::string treePath) 
  {
    m_actorIdMap->insert({ actorId, char_string(treePath.begin(), treePath.end(),char_allocator(m_segment.get_segment_manager())) });
    auto x = m_segment.get_free_memory();
    std::cout << x << std::endl;
  };

  void resetDebugBlackboard(const std::unordered_map<std::string, std::string>& newBlackboard)
  {
    m_blackBoardMap->clear();
    for (auto& [key, value] : newBlackboard) {
      m_blackBoardMap->insert({ ipc::basic_string<char>(key.c_str()), ipc::basic_string<char>(value.c_str()) });
    }
  }

};
  