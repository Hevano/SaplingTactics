#include "AIManager.h"
#include "BehaviourNodes.h"

#include "ArborMasterAdapter.h"

AIManager& AIManager::getInstance()
{
  static AIManager instance;
  instance_ = &instance;
  return *instance_;
}

void AIManager::addUnit(Unit unit)
{
  m_units.emplace(unit.id, std::make_shared<Unit>(unit));
  loadBTree(std::to_string(unit.id));
  m_trees[unit.id] = &m_cachedTrees[std::to_string(unit.id)];
  m_trees[unit.id]->actor = m_units[unit.id];
  m_teams[unit.team].emplace(unit.id);
}

void AIManager::removeUnit(Unit& unit) {
  m_trees.erase(unit.id);
  m_teams[unit.team].erase(unit.id);
  m_units.erase(unit.id);
}

std::unordered_map<UnitId, std::shared_ptr<Unit>>& AIManager::getUnits()
{
  return m_units;
}

const std::unordered_set<UnitId>& AIManager::getTeamIds(Unit::TeamEnum team) const
{
  return m_teams.at(team);
}

void AIManager::tick()
{
  for (auto& [id, tree] : m_trees) {
    tree->tick();
  }
}

BehaviourTree AIManager::loadBTree(const std::string& path)
{
  Adapter a;

  auto x = a.loadTree("C:\\Users\\Evano\\source\\repos\\ArborMaster\\build\\treeSample.json");

  if (path == "1") {
    m_cachedTrees[path] = BehaviourTree();
    m_cachedTrees[path].m_root =
        std::make_shared<SequenceNode>(&m_cachedTrees[path]);
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<AttackTargetNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<ChaseNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<MeleeAttackNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<WaitStartNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<WaitNode>(&m_cachedTrees[path]));
    return m_cachedTrees[path];
  } else {
    m_cachedTrees[path] = BehaviourTree();
    m_cachedTrees[path].m_root =
        std::make_shared<SequenceNode>(&m_cachedTrees[path]);
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<WanderTargetNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<MoveNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<WaitStartNode>(&m_cachedTrees[path]));
    m_cachedTrees[path].m_root->children.push_back(
        std::make_shared<WaitNode>(&m_cachedTrees[path]));
    return m_cachedTrees[path];
  }
}
