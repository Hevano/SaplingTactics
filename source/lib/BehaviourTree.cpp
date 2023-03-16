#include "BehaviourTree.h"
#include "Unit.h"

#include "raylib-cpp.hpp"

#include <unordered_set>
#include <typeinfo>
#include <format>

std::shared_ptr<BehaviourNode> BehaviourTree::copyTree(BehaviourTree& newTree, const std::shared_ptr<BehaviourNode> root) const
{
  std::shared_ptr<BehaviourNode> newRoot = root->clone(&newTree);
  newRoot->children.reserve(root->children.size());
  for (const auto& child : root->children) {
    newRoot->children.push_back(copyTree(newTree, child));
  }
  return newRoot;
}

BehaviourTree::BehaviourTree(const BehaviourTree& bt) 
  : blackboard(bt.blackboard)
  , actor(bt.actor)
  , debugPath(bt.debugPath)
{
  m_root = bt.copyTree(*this, bt.m_root);
}

void BehaviourTree::tick() {
  if (m_current && m_current->status == Status::Running) {
    m_current->evaluate();
  } else {
    m_root->evaluate();
  }
}

void BehaviourTree::setCurrent(BehaviourNode* newCurrent)
{
  m_current = newCurrent;
}

std::unordered_map<std::string, std::string> BehaviourTree::getStringBlackboard()
{
  std::unordered_map<std::string, std::string> map;

  for (auto& [key, value] : blackboard) {
    if (!value.has_value()) continue;

    map.emplace(getStringBlackboardKey(key, value));
  }

  return map;
}

std::pair<std::string, std::string> BehaviourTree::getStringBlackboardKey(const std::string& key)
{
  if (!blackboard.contains(key)) return std::pair<std::string, std::string>(key, ""); //removes the key from IPC
  return getStringBlackboardKey(key, blackboard.at(key));
}

unsigned int BehaviourTree::getActorId() const
{
  auto& a = *(actor.lock());
  return a.id;
}

std::pair<std::string, std::string> BehaviourTree::getStringBlackboardKey(const std::string& key, const std::any& value)
{
  if (value.type() == typeid(int)) {
    return std::make_pair(key, std::to_string(std::any_cast<int>(value)));
  }
  if (value.type() == typeid(unsigned int)) {
    return std::make_pair(key, std::to_string(std::any_cast<unsigned int>(value)));
  }
  else if (value.type() == typeid(float)) {
    return std::make_pair(key, std::to_string(std::any_cast<float>(value)));

  }
  else if ((value.type() == typeid(std::string))) {
    return std::make_pair(key, std::any_cast<std::string>(value));
  }
  else if ((value.type() == typeid(raylib::Vector2))) {
    raylib::Vector2 v = std::any_cast<raylib::Vector2>(value);
    return std::make_pair(key, std::format("x: {}, y: {}", v.x, v.y));
  }
  else {
    //If we have not prepared a value-string conversion, use the type name
    return std::make_pair(key, value.type().name());
  }
}

BehaviourNode::BehaviourNode(BehaviourTree* tree, unsigned int id)
  : tree(tree)
  , status(Status::Failure)
  , nodeId(id)
{
}

std::shared_ptr<BehaviourNode> BehaviourNode::clone(BehaviourTree* bt)
{
  return std::make_shared<BehaviourNode>(BehaviourNode(bt, nodeId));
}
