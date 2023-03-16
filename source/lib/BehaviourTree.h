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
  unsigned int nodeId;

  Status status;

  std::vector<std::shared_ptr<BehaviourNode>> children;

  BehaviourTree* const tree;

  BehaviourNode(BehaviourTree* tree, unsigned int id = 0);
  BehaviourNode(const BehaviourNode& bn) = default;
  virtual ~BehaviourNode() = default;
  virtual Status evaluate() { return Status::Failure; };
  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt);
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
  std::string debugPath;

private:
  //returns the root node of a copied tree
  std::shared_ptr<BehaviourNode> copyTree(BehaviourTree& newTree, const std::shared_ptr<BehaviourNode> root) const;

public:
  BehaviourTree() = default;

  BehaviourTree(const BehaviourTree& bt);

  void tick();
  void setCurrent(BehaviourNode* newCurrent);
  std::unordered_map<std::string, std::string> getStringBlackboard();
  std::pair<std::string, std::string> getStringBlackboardKey(const std::string& key, const std::any& value);
  std::pair<std::string, std::string> getStringBlackboardKey(const std::string& key);
  unsigned int getActorId() const;
};