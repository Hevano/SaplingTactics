#include "AIManager.h"
#include "BehaviourNodes.h"

#include <sstream>

AIManager& AIManager::getInstance()
{
  static AIManager instance;
  instance_ = &instance;
  return *instance_;
}

//TODO: Replace with a factory class
void AIManager::addUnit(Unit unit)
{
  std::string path = "C:\\Users\\Evano\\source\\repos\\ArborMaster\\build\\wanderDesignExport.json";
  m_units.emplace(unit.id, std::make_shared<Unit>(unit));
  loadBTree(path, unit.id);
  d.createDebugActor(unit.id, path);
  m_trees[unit.id].actor = m_units[unit.id];
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
  //If a new thing is selected, we have to change out the blackboard (could require some synchronization)
  if (d.tick()) {
    auto tree = m_trees[d.getCurrentActorId()];
    std::ostringstream oss;
    std::unordered_map<std::string, std::string> stringBlackboard = tree.getStringBlackboard();
    d.resetDebugBlackboard(stringBlackboard);
  }
  for (auto& [id, tree] : m_trees) {
    d.updateNodeStatus(id, 0,0);
    tree.tick();
  }
}

void AIManager::updateDebugger(UnitId unitId, const std::string& key)
{
  if (d.getCurrentActorId() != unitId) return;
  d.updateDebugBlackboard(unitId, m_trees[unitId].getStringBlackboardKey(key));
}

BehaviourTree& AIManager::loadBTree(const std::string& path, UnitId unitId)
{
  if (!m_cachedTrees.contains(path)) {
    //TODO: Consider changing this from a class to just a namespace with functions, as we dont track any state
    Adapter a;

    

    m_cachedTrees[path] = BehaviourTree();

    auto x = a.loadTree(path, m_cachedTrees[path].debugPath);

    m_cachedTrees[path].m_root = makeNode(m_cachedTrees[path], x->name, x->nodeId);

    for (auto child : x->children) {
      addChildren(m_cachedTrees[path], m_cachedTrees[path].m_root, child);
    }
  }

  m_trees.emplace(unitId, m_cachedTrees[path]);
  return m_trees[unitId];
}

void AIManager::addChildren(BehaviourTree& tree, std::shared_ptr<BehaviourNode> parent, std::shared_ptr<TreeDesignNode> design) {
  parent->children.push_back(makeNode(tree, design->name, design->nodeId));
  for (auto child : design->children) {
    addChildren(tree, parent->children.back(), child);
  }
}

std::shared_ptr<BehaviourNode> AIManager::makeNode(BehaviourTree& tree, const std::string& name, unsigned int id)
{
  static std::unordered_map<std::string, std::function<std::shared_ptr<BehaviourNode>(BehaviourTree&, unsigned int)>> constructors;

  if (constructors.size() == 0) {
    constructors.emplace("SequenceNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<SequenceNode>(&t, i); });
    constructors.emplace("SelectorNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<SelectorNode>(&t, i); });
    constructors.emplace("WanderTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WanderTargetNode>(&t, i); });
    constructors.emplace("AttackTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<AttackTargetNode>(&t, i); });
    constructors.emplace("ChaseNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<ChaseNode>(&t, i); });
    constructors.emplace("MeleeAttackNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<MeleeAttackNode>(&t, i); });
    constructors.emplace("MoveNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<MoveNode>(&t, i); });
    constructors.emplace("WaitStartNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WaitStartNode>(&t, i); });
    constructors.emplace("WaitNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WaitNode>(&t, i); });
  }
  

  return constructors[name](tree, id);
}
