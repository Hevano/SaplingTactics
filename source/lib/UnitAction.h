#pragma once
#include "BehaviourTree.h"

#include <functional>
#include <any>
#include <vector>
#include <unordered_map>

class Unit;

struct UnitAction {
  enum ActionType {
    MeleeAttack,
    RangedAttack,
    Aura,
    Generic
  };

  Status status = Status::Success;
  bool active = false;

  //Additional vector of any data the action may need
  std::vector<std::any> data;

  std::function<Status(Unit&, UnitAction&)> effect;
  Status tick(Unit& u);
  void start(Unit& u);
};