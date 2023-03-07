#include "AIManager.h"
#include "BehaviourNodes.h"

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
    d.updateDebugStatus(id, 0,0);
    tree->tick();
  }
}

BehaviourTree AIManager::loadBTree(const std::string& path)
{
  Adapter a;

  auto x = a.loadTree("C:\\Users\\Evano\\source\\repos\\ArborMaster\\build\\hunterTree.json");

  if (path == "1") {
    m_cachedTrees[path] = BehaviourTree();

    m_cachedTrees[path].m_root = makeNode(m_cachedTrees[path], x->name, x->nodeId);

    for (auto child : x->children) {
      addChildren(m_cachedTrees[path], m_cachedTrees[path].m_root, child);
    }


    /*m_cachedTrees[path].m_root =
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
        std::make_shared<WaitNode>(&m_cachedTrees[path]));*/
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
