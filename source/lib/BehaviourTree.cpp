#include "BehaviourTree.h"

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

BehaviourNode::BehaviourNode(BehaviourTree* tree, unsigned int id)
  : tree(tree)
  , status(Status::Failure)
  , nodeId(id)
{
}
