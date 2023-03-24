#pragma once
#include "BehaviourTree.h"
#include "raylib.hpp"
#include "Unit.h"
#include "AIManager.h"

#include <queue>

//[ArborMaster]SequenceNode|99|
struct SequenceNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~SequenceNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<SequenceNode>(SequenceNode(bt, nodeId));
  }
  
  int currentChild = 0;
  Status evaluate() override
  {
    for (int i = currentChild; i < children.size(); i++) {
      auto childStatus = children[i]->evaluate();
      currentChild = i;
      if (childStatus == Status::Running) {
        return setStatus(childStatus);
      } else if (childStatus == Status::Failure) {
        currentChild = 0; //Sequence failed, reset to start
        return setStatus(childStatus);
      }
    }
    currentChild = 0;  // Sequence succeeded, reset to start
    return setStatus(Status::Success);
  }
};

//[ArborMaster]SelectorNode|99|
struct SelectorNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~SelectorNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<SelectorNode>(SelectorNode(bt, nodeId));
  }

  int currentChild = 0;
  Status evaluate() override
  {
    for (int i = currentChild; i < children.size(); i++) {
      auto childStatus = children[i]->evaluate();
      currentChild = i;
      if (childStatus == Status::Running) {
        return setStatus(childStatus);
      } else if (childStatus == Status::Success) {
        currentChild = 0;  // Sequence succeeded, reset to start
        return setStatus(childStatus);
      }
    }
    currentChild = 0;  // Selection failed, reset to start
    return setStatus(Status::Failure);
  }
};

//[ArborMaster]InverterNode|1|
struct InverterNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~InverterNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<InverterNode>(InverterNode(bt, nodeId));
  }

  Status evaluate() override
  {
    auto childStatus = children[0]->evaluate();
    switch (childStatus) {
    case Status::Failure:
      return setStatus(Status::Success);
    case Status::Success:
      return setStatus(Status::Failure);
    default:
      return setStatus(Status::Running);
    }
  }
};

//[ArborMaster]WanderTargetNode|0|MoveTarget|
struct WanderTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WanderTargetNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<WanderTargetNode>(WanderTargetNode(bt, nodeId));
  }

  Status evaluate() override
  {
    if (!tree->blackboard.contains("MoveTarget")
        || std::any_cast<raylib::Vector2>(tree->blackboard["MoveTarget"]).CheckCollision(tree->actor.lock()->rect))
    {
      raylib::Vector2 v(GetRandomValue(0, 1280), GetRandomValue(0, 720));
      tree->blackboard["MoveTarget"] = std::make_any<raylib::Vector2>(v);
      AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "MoveTarget");
    }
    return setStatus(Status::Success);
  }
};

//[ArborMaster]TargetInRangeNode|0|AttackTarget|
struct TargetInRangeNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~TargetInRangeNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<TargetInRangeNode>(TargetInRangeNode(bt, nodeId));
  }

  float radius = 500.f;
  Status evaluate() override
  {
    auto& actor = *(tree->actor.lock());
    auto enemyTeam = (actor.team == Unit::Team::Player) ? Unit::Team::Computer : Unit::Team::Player;
    auto units = AIManager::getInstance().getUnits();
    for (auto& id : AIManager::getInstance().getTeamIds(enemyTeam)) {
      auto& unit = units[id];
      if (unit->active && actor.getPos().Distance(unit->getPos()) < radius)
      {
        tree->blackboard["AttackTarget"] = std::make_any<UnitId>(unit->id);
        AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "AttackTarget");
        return setStatus(Status::Success);
      }
    }
    return setStatus(Status::Failure);
  }
};

//[ArborMaster]NearestTargetNode|0|AttackTarget|
struct NearestTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~NearestTargetNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<NearestTargetNode>(NearestTargetNode(bt, nodeId));
  }

  float radius = 500.f;
  Status evaluate() override
  {
    auto& actor = *(tree->actor.lock());
    auto enemyTeam = (actor.team == Unit::Team::Player) ? Unit::Team::Computer : Unit::Team::Player;
    auto units = AIManager::getInstance().getUnits();
    auto distanceComparison = [&actor](const Unit* lhs, const Unit* rhs)
    {
      return actor.getPos().Distance(lhs->getPos()) > actor.getPos().Distance(rhs->getPos());
    };
    std::priority_queue<Unit*, std::vector<Unit*>, std::function<bool(const Unit*, const Unit*)>> targets(distanceComparison);
    for (auto& id : AIManager::getInstance().getTeamIds(enemyTeam)) {
      auto& unit = units[id];
      if (unit->active)
      {
        targets.push(unit.get());
      }
    }

    if (targets.empty()) {
      return setStatus(Status::Failure);
    }

    tree->blackboard["AttackTarget"] = std::make_any<UnitId>(targets.top()->id);
    AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "AttackTarget");
    return setStatus(Status::Success);
  }
};


//[ArborMaster]ChaseNode|0|AttackTarget|
struct ChaseNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~ChaseNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<ChaseNode>(ChaseNode(bt, nodeId));
  }

  float radius = 10.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return setStatus(Status::Failure);
    }

    auto attackTargetId = std::any_cast<UnitId>(tree->blackboard["AttackTarget"]);
    raylib::Vector2 targetPos = AIManager::getInstance().getUnits()[attackTargetId]->getPos();
    auto& actor = *(tree->actor.lock());
    if (actor.getPos().Distance(targetPos) < radius) {
      return setStatus(Status::Success);
    } else {
      actor.setMovement(targetPos);
      tree->setCurrent(this);
      return setStatus(Status::Running);
    }
  }
};

//[ArborMaster]ApproachTargetNode|0|AttackTarget|
struct ApproachTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~ApproachTargetNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<ApproachTargetNode>(ApproachTargetNode(bt, nodeId));
  }

  float radius = 10.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return setStatus(Status::Failure);
    }

    auto& actor = *(tree->actor.lock());
    auto attackTargetId = std::any_cast<UnitId>(tree->blackboard["AttackTarget"]);
    raylib::Vector2 targetPos = AIManager::getInstance().getUnits()[attackTargetId]->getPos();

    auto midpoint = actor.getPos() + (targetPos - actor.getPos()) * 0.5f;
    tree->blackboard["MoveTarget"] = std::make_any<raylib::Vector2>(midpoint);
    AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "MoveTarget");


    return setStatus(Status::Success);
  }
};

//[ArborMaster]MeleeAttackNode|0|AttackTarget|
struct MeleeAttackNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~MeleeAttackNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<MeleeAttackNode>(MeleeAttackNode(bt, nodeId));
  }

  float radius = 50.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return setStatus(Status::Failure);
    }
    auto& actor = *(tree->actor.lock());
    return setStatus(actor.takeAction(UnitAction::MeleeAttack));
  }
};

//[ArborMaster]RangedAttackNode|0|AttackTarget|
struct RangedAttackNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~RangedAttackNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<RangedAttackNode>(RangedAttackNode(bt, nodeId));
  }

  float radius = 50.f;
  Status evaluate() override
  {
    if (!tree->blackboard.contains("AttackTarget")) {
      return setStatus(Status::Failure);
    }
    auto& actor = *(tree->actor.lock());

    return setStatus(actor.takeAction(UnitAction::RangedAttack));
  }
};

//[ArborMaster]MoveNode|0|MoveTarget|
struct MoveNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~MoveNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<MoveNode>(MoveNode(bt, nodeId));
  }

  Status evaluate() override
  {
    if (!tree->blackboard.contains("MoveTarget"))
    {
      return setStatus(Status::Failure);
    }
    
    auto moveTarget = std::any_cast<raylib::Vector2>(tree->blackboard["MoveTarget"]);
    auto& actor = *(tree->actor.lock());

    if (actor.rect.CheckCollision(moveTarget))
    {
      actor.clearMovement();
      tree->setCurrent(nullptr);
      return setStatus(Status::Success);
    } else {
      actor.setMovement(moveTarget);
      tree->setCurrent(this);
      return setStatus(Status::Running);
    }
  }
};


//[ArborMaster]WaitStartNode|0|WaitTimestamp|
struct WaitStartNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitStartNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<WaitStartNode>(WaitStartNode(bt, nodeId));
  }

  Status evaluate() override
  {
    tree->blackboard["WaitTimestamp"] = std::make_any<float>(GetTime());
    AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "WaitTimestamp");
    return setStatus(Status::Success);
  }
};

//[ArborMaster]WaitNode|0|WaitTimestamp|
struct WaitNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~WaitNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<WaitNode>(WaitNode(bt, nodeId));
  }

  Status evaluate() override
  {
    if (!tree->blackboard.contains("WaitTimestamp")) {
      return setStatus(Status::Success);
    }

    auto startTime = std::any_cast<float>(tree->blackboard["WaitTimestamp"]);
    if (status != Status::Running) {
      tree->setCurrent(this);
      return setStatus(Status::Running);
    } else if (startTime + 5 < GetTime()) {  // Wait for 5 seconds (hard coded)
      tree->blackboard.erase("WaitTimestamp");
      AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "WaitTimestamp");
      tree->setCurrent(nullptr);
      return setStatus(Status::Success);
    }
  }
};

//[ArborMaster]EnemyProximityNode|0|ProximityTarget|
struct EnemyProximityNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~EnemyProximityNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<EnemyProximityNode>(EnemyProximityNode(bt, nodeId));
  }

  Status evaluate() override
  {
    auto& actor = *(tree->actor.lock());
    auto enemyTeam = (actor.team == Unit::Team::Player) ? Unit::Team::Computer : Unit::Team::Player;
    auto& units = AIManager::getInstance().getUnits();
    for (auto& id : AIManager::getInstance().getTeamIds(enemyTeam)) {
      auto& unit = units[id];
      if (unit->active && actor.getPos().Distance(unit->getPos()) < unit->stats[Unit::Stat::Speed] * 2)
      {
        tree->blackboard["ProximityTarget"] = std::make_any<UnitId>(unit->id);
        AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "ProximityTarget");
        return setStatus(Status::Success);
      }
    }
    return setStatus(Status::Failure);
  }
};

//[ArborMaster]FleeTargetNode|0|MoveTarget|ProximityTarget
struct FleeTargetNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~FleeTargetNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<FleeTargetNode>(FleeTargetNode(bt, nodeId));
  }

  Status evaluate() override
  {
    if (!tree->blackboard.contains("ProximityTarget")) {
      return setStatus(Status::Failure);
    }
   
    auto fleeTargetId = std::any_cast<UnitId>(tree->blackboard["ProximityTarget"]);

    auto threatPos = AIManager::getInstance().getUnits()[fleeTargetId]->getPos();
    auto& actor = *(tree->actor.lock());

    raylib::Vector2 v = actor.getPos() + (actor.getPos() - threatPos).Normalize() * actor.stats[Unit::Stat::Speed];
    tree->blackboard["MoveTarget"] = std::make_any<raylib::Vector2>(v);
    AIManager::getInstance().updateUnitDebugger(tree->getActorId(), "MoveTarget");
    return setStatus(Status::Success);
  }
};

//[ArborMaster]IsTargetStrongerNode|0|ProximityTarget|
struct IsTargetStrongerNode : BehaviourNode
{
  using BehaviourNode::BehaviourNode;
  virtual ~IsTargetStrongerNode() override = default;

  virtual std::shared_ptr<BehaviourNode> clone(BehaviourTree* bt) override
  {
    return std::make_shared<IsTargetStrongerNode>(IsTargetStrongerNode(bt, nodeId));
  }

  Status evaluate() override
  {
    
    std::string targetName = "";
    if (tree->blackboard.contains("AttackTarget")) {
      targetName = "AttackTarget";
    }
    else if (tree->blackboard.contains("ProximityTarget")) {
      targetName = "ProximityTarget";
    }

    if (targetName.empty()) {
      return setStatus(Status::Failure);
    }

    auto& actor = *(tree->actor.lock());
    auto targetId = std::any_cast<UnitId>(tree->blackboard[targetName]);
    auto& target = AIManager::getInstance().getUnits()[targetId];
    if (target->stats[Unit::Stat::Damage] + target->stats[Unit::Stat::Health] > actor.stats[Unit::Stat::Damage] + actor.stats[Unit::Stat::Health]) {
      return setStatus(Status::Success);
    }
    else {
      return setStatus(Status::Failure);
    }
    
  }
};


