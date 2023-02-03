#pragma once
#include "BehaviourTree.h"

struct SequenceNode : BehaviourNode
{
  int currentChild = 0;
  Status evaluate() override
  {
    for (int i = currentChild; i < children.size(); i++) {
      auto childStatus = children[i]->evaluate();
      if (childStatus == Status::Running || childStatus == Status::Failure) {
        return status = childStatus;
      }
    }
    return status = Status::Success;
  }
};

struct SelectorNode : BehaviourNode
{
};

