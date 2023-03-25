#pragma once

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <string>
#include <fstream>
#include <iostream>

namespace ArborMasterAdapter {
  namespace ipc = boost::interprocess;

  struct ActorUpdate {
    unsigned int nodeId;
    unsigned int actorId;
    unsigned int status;
  };

  class Debugger {
  private:
    //Max number of bytes our shared memory buffer can hold
    const int MAX_BYTES = 65536;

    typedef ipc::allocator<char, ipc::managed_shared_memory::segment_manager> char_allocator;
    typedef ipc::basic_string<char, std::char_traits<char>, char_allocator> char_string;

    typedef ipc::allocator<std::pair<const unsigned int, char_string>, ipc::managed_shared_memory::segment_manager> actorid_allocator;
    typedef ipc::flat_map<unsigned int, char_string, std::less<unsigned int>, actorid_allocator> actorid_map_type;

    typedef ipc::allocator<std::pair<const char_string, char_string>, ipc::managed_shared_memory::segment_manager> bb_allocator_type;
    typedef ipc::flat_map<char_string, char_string, std::less<char_string>, bb_allocator_type> bb_map_type;

    ipc::managed_shared_memory m_segment;
    std::unique_ptr<bb_map_type> m_blackBoardMap;
    std::unique_ptr<actorid_map_type> m_actorIdMap;
    std::unique_ptr <ipc::message_queue> m_actorIdMessageQueue;
    std::unique_ptr <ipc::message_queue> m_nodeUpdateMessageQueue;

    std::unique_ptr <char_allocator> m_charAllocator;

    unsigned int m_currentActorId;

  public:

    //Initiates IPC with the designer, returning false if it fails
    bool init();

    //returns true if actor has changed, in which case blackboard should be updated
    bool tick();

    inline unsigned int getCurrentActorId() const
    {
      return m_currentActorId;
    }

    ~Debugger();

    //Updates the running status of a node
    void updateNodeStatus(unsigned int nodeId, unsigned int actorId, unsigned int status);

    //Updates the blackboard value of an actor
    void updateDebugBlackboard(unsigned int actorId, std::string key, std::string value);
    void updateDebugBlackboard(unsigned int actorId, std::pair<std::string, std::string> keyValue);

    //declares that there is an actor who uses a tree at the path
    void createDebugActor(unsigned int actorId, std::string treePath);

    void resetDebugBlackboard(const std::unordered_map<std::string, std::string>& newBlackboard);

  };
}