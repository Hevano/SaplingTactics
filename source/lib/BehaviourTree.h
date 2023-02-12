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

  BehaviourTree* const tree;

  BehaviourNode(BehaviourTree* tree);
  BehaviourNode(const BehaviourNode& bn) = default;
  virtual ~BehaviourNode() = default;
  virtual Status evaluate() { return Status::Failure; };
};


//Needs deep copy
class BehaviourTree
{
friend class AIManager;

private:
  std::shared_ptr<BehaviourNode> m_root;
  BehaviourNode* m_current = nullptr;

public:
  std::unordered_map<std::string, std::any> blackboard;
  std::weak_ptr<Unit> actor; //Potential issue, if we need to access Unit-subclass specific members

public:
  void tick();
  void setCurrent(BehaviourNode* newCurrent);
};