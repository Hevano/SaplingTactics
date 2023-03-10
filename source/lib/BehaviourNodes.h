#pragma once
#include "BehaviourTree.h"
#include "raylib.hpp"
#include "Unit.h"
#include "AIManager.h"

//[ArborMaster]SequenceNode|99|
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

//[ArborMaster]SelectorNode|99|
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

//[ArborMaster]WanderTargetNode|0|MoveTarget|
struct WanderTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WanderTargetNode() override = default;

  Status evaluate() override
  {
    if (!tree->blackboard.contains("MoveTarget")
        || std::any_cast<raylib::Vector2>(tree->blackboard["MoveTarget"]).CheckCollision(tree->actor.lock()->rect))
    {
      raylib::Vector2 v(GetRandomValue(0, 1280), GetRandomValue(0, 720));
      tree->blackboard["MoveTarget"] = std::make_any<raylib::Vector2>(v);
      AIManager::getInstance().updateDebugger(tree->getActorId(), "MoveTarget");
    }
    return status = Status::Success;
  }
};

//[ArborMaster]AttackTargetNode|0|AttackTarget|
struct AttackTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~AttackTargetNode() override = default;
  float radius = 300.f;
  Status evaluate() override
  {
    auto& actor = *(tree->actor.lock());
    auto enemyTeam = (actor.team == Unit::Player) ? Unit::Computer : Unit::Player;
    auto units = AIManager::getInstance().getUnits();
    for (auto& id : AIManager::getInstance().getTeamIds(enemyTeam)) {
      auto& unit = units[id];
      if (unit->active && actor.rect.GetPosition().Distance(unit->rect.GetPosition()) < radius)
      {
        tree->blackboard["AttackTarget"] = std::make_any<UnitId>(unit->id);
        AIManager::getInstance().updateDebugger(tree->getActorId(), "AttackTarget");
        return status = Status::Success;
      }
    }
    return status = Status::Failure;
  }
};


//[ArborMaster]ChaseNode|0|AttackTarget|
struct ChaseNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~ChaseNode() override = default;
  float radius = 10.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return status = Status::Failure;
    }

    auto attackTargetId = std::any_cast<UnitId>(tree->blackboard["AttackTarget"]);
    raylib::Vector2 targetPos = AIManager::getInstance().getUnits()[attackTargetId]->rect.GetPosition();
    auto& actor = *(tree->actor.lock());
    if (actor.rect.GetPosition().Distance(targetPos) < radius) {
      return status = Status::Success;
    } else {
      actor.setMovement(targetPos);
      tree->setCurrent(this);
      return status = Status::Running;
    }
  }
};

//[ArborMaster]MeleeAttackNode|0|AttackTarget|
struct MeleeAttackNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~MeleeAttackNode() override = default;
  float radius = 50.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return status = Status::Failure;
    }

    auto attackTargetId = std::any_cast<UnitId>(tree->blackboard["AttackTarget"]);
    auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
    auto& actor = *(tree->actor.lock());

    if (actor.rect.GetPosition().Distance(targetUnit->rect.GetPosition()) > radius) {
      return status = Status::Failure;
    } 

    actor.adjustTargetStat(Unit::Health, 1, *targetUnit);
    return status = Status::Success;
  }
};

//[ArborMaster]MoveNode|0|MoveTarget|
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


//[ArborMaster]WaitStartNode|0|WaitTimestamp|
struct WaitStartNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitStartNode() override = default;

  Status evaluate() override
  {
    tree->blackboard["WaitTimestamp"] = std::make_any<float>(GetTime());
    AIManager::getInstance().updateDebugger(tree->getActorId(), "WaitTimestamp");
    return status = Status::Success;
  }
};

//[ArborMaster]WaitNode|0|WaitTimestamp|
struct WaitNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitNode() override = default;

  Status evaluate() override
  {
    if (!tree->blackboard.contains("WaitTimestamp")) {
      return status = Status::Success;
    }

    auto startTime = std::any_cast<float>(tree->blackboard["WaitTimestamp"]);
    if (status != Status::Running) {
      tree->setCurrent(this);
      return status = Status::Running;
    } else if (startTime + 5 < GetTime()) {  // Wait for 5 seconds (hard coded)
      tree->blackboard.erase("WaitTimestamp");
      AIManager::getInstance().updateDebugger(tree->getActorId(), "WaitTimestamp");
      tree->setCurrent(nullptr);
      return status = Status::Success;
    }
  }
};


