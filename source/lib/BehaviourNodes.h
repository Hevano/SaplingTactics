#pragma once
#include "BehaviourTree.h"
#include "raylib.hpp"
#include "Unit.h"

struct SequenceNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~SequenceNode() override = default;
  
  int currentChild = 0;
  Status evaluate() override
  {
    for (int i = currentChild; i < children.size(); i++) {
      auto childStatus = children[i]->evaluate();
      currentChild = i;
      if (childStatus == Status::Running) {
        return status = childStatus;
      } else if (childStatus == Status::Failure) {
        currentChild = 0; //Sequence failed, reset to start
        return status = childStatus;
      }
    }
    currentChild = 0;  // Sequence succeeded, reset to start
    return status = Status::Success;
  }
};

struct SelectorNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~SelectorNode() override = default;

  int currentChild = 0;
  Status evaluate() override
  {
    for (int i = currentChild; i < children.size(); i++) {
      auto childStatus = children[i]->evaluate();
      currentChild = i;
      if (childStatus == Status::Running) {
        return status = childStatus;
      } else if (childStatus == Status::Success) {
        currentChild = 0;  // Sequence succeeded, reset to start
        return status = childStatus;
      }
    }
    currentChild = 0;  // Selection failed, reset to start
    return status = Status::Failure;
  }
};

struct WanderTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WanderTargetNode() override = default;

  Status evaluate() override
  {
    if (!tree->blackboard.contains("MoveTarget")
        || std::any_cast<raylib::Vector2>(tree->blackboard["MoveTarget"]).CheckCollision(tree->actor.lock()->rect))
    {
      tree->blackboard["MoveTarget"] = std::make_any<raylib::Vector2>(
          raylib::Vector2(GetRandomValue(0, 1280), GetRandomValue(0, 720)));
    }
    return status = Status::Success;
  }
};

struct AttackTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~AttackTargetNode() override = default;

  Status evaluate() override
  {
    
  }
};

struct MoveNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~MoveNode() override = default;

  Status evaluate() override
  {
    if (!tree->blackboard.contains("MoveTarget"))
    {
      return status = Status::Failure;
    }
    
    auto moveTarget = std::any_cast<raylib::Vector2>(tree->blackboard["MoveTarget"]);
    auto& actor = *(tree->actor.lock());

    if (actor.rect.CheckCollision(moveTarget))
    {
      actor.clearMovement();
      tree->setCurrent(nullptr);
      return status = Status::Success;
    } else {
      actor.setMovement(moveTarget);
      tree->setCurrent(this);
      return status = Status::Running;
    }
  }
};

struct WaitStartNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitStartNode() override = default;

  double timer = 0;
  Status evaluate() override
  {
    tree->blackboard["WaitStart"] = std::make_any<float>(GetTime());
    return status = Status::Success;
  }
};

struct WaitNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitNode() override = default;

  Status evaluate() override
  {
    if (!tree->blackboard.contains("WaitStart")) {
      return status = Status::Success;
    }

    auto startTime = std::any_cast<float>(tree->blackboard["WaitStart"]);
    if (status != Status::Running) {
      tree->setCurrent(this);
      return status = Status::Running;
    } else if (startTime + 5 < GetTime()) {  // Wait for 5 seconds (hard coded)
      tree->blackboard.erase("WaitStart");
      tree->setCurrent(nullptr);
      return status = Status::Success;
    }
  }
};


