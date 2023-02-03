#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <any>

enum class Status
{
  Success,
  Failure,
  Running
};

class BehaviourTree;
class Unit;

struct BehaviourNode
{
  Status status;

  std::vector<std::shared_ptr<BehaviourNode>> children;

  std::shared_ptr<BehaviourTree> tree;

  BehaviourNode(BehaviourTree& t);
  virtual ~BehaviourNode();
  virtual Status evaluate() = 0;
};

class BehaviourTree
{
private:
  std::shared_ptr<BehaviourNode> m_root;
  std::shared_ptr<BehaviourNode> m_current;

public:
  std::unordered_map<std::string, std::any> blackboard;
  std::shared_ptr<Unit> actor; //Potential issue, if we need to access Unit-subclass specific members

public:
  void tick();
  void setCurrent(BehaviourNode* newCurrent);
};