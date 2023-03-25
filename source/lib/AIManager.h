#pragma once
#include "Unit.h"
#include "BehaviourTree.h"

#include <adapter/Debugger.h>
#include <adapter/Adapter.h>

#include <unordered_map>
#include <unordered_set>

using namespace ArborMasterAdapter;

class AIManager
{
  //Singleton Pattern
private:
  static inline AIManager* instance_ = nullptr;
  AIManager() {
    std::unordered_map<unsigned int, std::string> mappings = { {0, "fake unit"} };
    d.init();
  };
  ~AIManager() = default;

public:
  // Illegal to clone or assign singleton
  AIManager(AIManager& other) = delete;
  void operator=(const AIManager&) = delete;
  static AIManager& getInstance();

private:
  Debugger d;


  std::unordered_map<UnitId, std::shared_ptr<Unit>> m_units;
  //Handy lookups
  std::unordered_map<UnitId, BehaviourTree> m_trees;
  std::unordered_map<Unit::Team, std::unordered_set<UnitId>> m_teams;
  std::unordered_map<UnitId, std::string> m_unitTreePaths;

  //Loads trees from disk once and caches here
  std::unordered_map<std::string, BehaviourTree> m_cachedTrees;

public:
  Unit& addUnit(UnitId id, Unit::Team team, const std::string& treePath);

  void removeUnit(Unit& unit);

  std::unordered_map<UnitId, std::shared_ptr<Unit>>& getUnits();
  const std::unordered_set<UnitId>& getTeamIds(Unit::Team team) const;
  const std::unordered_map<std::string, std::any>& getUnitBlackboard(UnitId id);

  void tick();

  void reset();

  void updateUnitDebugger(UnitId unitId, const std::string& key);
  void updateNodeDebugger(unsigned int nodeId, UnitId unitId, Status status);

private:
  BehaviourTree& loadBTree(const std::string& path, UnitId unitId);
  void addChildren(BehaviourTree& tree, std::shared_ptr<BehaviourNode> parent, std::shared_ptr<TreeDesignNode> design);
  std::shared_ptr<BehaviourNode> makeNode(BehaviourTree& tree, const std::string& path, unsigned int id);
};