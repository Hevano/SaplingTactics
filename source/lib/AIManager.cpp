#include "AIManager.h"
#include "BehaviourNodes.h"

#include <sstream>

AIManager& AIManager::getInstance()
{
  static AIManager instance;
  instance_ = &instance;
  return *instance_;
}

Unit& AIManager::addUnit(UnitId id, Unit::Team team, const std::string& treePath)
{
  m_units.emplace(id, std::make_shared<Unit>());
  loadBTree(treePath, id);
  d.createDebugActor(id, m_cachedTrees[treePath].debugPath);
  m_trees[id].actor = m_units[id];
  m_teams[team].emplace(id);

  return *m_units[id];
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

const std::unordered_set<UnitId>& AIManager::getTeamIds(Unit::Team team) const
{
  return m_teams.at(team);
}

const std::unordered_map<std::string, std::any>& AIManager::getUnitBlackboard(UnitId id)
{
  return m_trees[id].blackboard;
}

void AIManager::tick()
{
  //If a new thing is selected, we have to change out the blackboard (could require some synchronization)

  try {
    if (d.tick()) {
      auto tree = m_trees[d.getCurrentActorId()];
      std::ostringstream oss;
      std::unordered_map<std::string, std::string> stringBlackboard = tree.getStringBlackboard();
      d.resetDebugBlackboard(stringBlackboard);
    }
  }
  catch (ipc::interprocess_exception& ex) {
    std::cout << "Error: " << ex.what() << std::endl;
  }
  
  for (auto& [id, tree] : m_trees) {
    tree.tick();
  }
}

void AIManager::updateUnitDebugger(UnitId unitId, const std::string& key)
{
  if (d.getCurrentActorId() != unitId) return;
  d.updateDebugBlackboard(unitId, m_trees[unitId].getStringBlackboardKey(key));
}

void AIManager::updateNodeDebugger(unsigned int nodeId, UnitId unitId, Status status)
{
  unsigned int i = static_cast<unsigned int>(status);
  d.updateNodeStatus(nodeId, unitId, i);
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
    constructors.emplace("InverterNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<InverterNode>(&t, i); });
    constructors.emplace("SelectorNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<SelectorNode>(&t, i); });
    constructors.emplace("WanderTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WanderTargetNode>(&t, i); });
    constructors.emplace("TargetInRangeNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<TargetInRangeNode>(&t, i); });
    constructors.emplace("ChaseNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<ChaseNode>(&t, i); });
    constructors.emplace("MeleeAttackNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<MeleeAttackNode>(&t, i); });
    constructors.emplace("RangedAttackNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<RangedAttackNode>(&t, i); });
    constructors.emplace("MoveNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<MoveNode>(&t, i); });
    constructors.emplace("WaitStartNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WaitStartNode>(&t, i); });
    constructors.emplace("WaitNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<WaitNode>(&t, i); });
    constructors.emplace("EnemyProximityNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<EnemyProximityNode>(&t, i); });
    constructors.emplace("FleeTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<FleeTargetNode>(&t, i); });
    constructors.emplace("NearestTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<NearestTargetNode>(&t, i); });
    constructors.emplace("ApproachTargetNode", [](BehaviourTree& t, unsigned int i) { return std::make_shared<ApproachTargetNode>(&t, i); });
  }
  

  return constructors[name](tree, id);
}
