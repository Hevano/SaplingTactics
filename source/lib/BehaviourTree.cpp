#include "BehaviourTree.h"

void BehaviourTree::tick() {
  if (m_current != nullptr && m_current->status == Status::Running) {
    m_current->evaluate();
  } else {
    m_root->evaluate();
  }
}

void BehaviourTree::setCurrent(BehaviourNode* newCurrent) {
  if (m_current != nullptr) throw std::exception("Attempt to set new current bNode when old current was still running");
  m_current = std::make_shared<BehaviourNode>(newCurrent);
}
