#pragma once
#include "Unit.h"
#include "BehaviourTree.h"

#include <unordered_map>
#include <unordered_set>

class AIManager
{
  //Singleton Pattern
private:
  static inline AIManager* instance_ = nullptr;
  AIManager() = default;
  ~AIManager() = default;

public:
  // Illegal to clone or assign singleton
  AIManager(AIManager& other) = delete;
  void operator=(const AIManager&) = delete;
  static AIManager& getInstance();

private:
  std::unordered_map<UnitId, std::shared_ptr<Unit>> m_units;
  //Handy lookups
  std::unordered_map<UnitId, BehaviourTree*> m_trees;
  std::unordered_map<Unit::TeamEnum, std::unordered_set<UnitId>> m_teams;

  //Loads trees from disk once and caches here
  std::unordered_map<std::string, BehaviourTree> m_cachedTrees;

public:
  void addUnit(Unit unit);

  //Template alternative
  /*template <typename UnitType, typename... Ts>
  void addUnit(Unit::TeamEnum team, Ts... args)
  {
    m_units.push_back(UnitType(UnitType::texturePath, &args...));
    m_trees[m_units.back().id] = std::move(loadBTree(UnitType::treePath));
  }*/

  void removeUnit(Unit& unit);

  std::unordered_map<UnitId, std::shared_ptr<Unit>>& getUnits();
  const std::unordered_set<UnitId>& getTeamIds(Unit::TeamEnum team) const;

  void tick();

private:
  BehaviourTree loadBTree(const std::string& path);
};