#include "EntityManager.h"

void EntityManager::tick()
{
  std::vector<int> toDestroy;
  for (int i = 0; i < m_entities.size(); i++) {
    if (!m_entities[i]->tick()) {
      toDestroy.push_back(i);
    }
  }

  for (auto i : toDestroy) {
    m_entities.erase(m_entities.begin() + i);
  }
}

void EntityManager::addEntity(std::shared_ptr<Entity> e)
{
  m_entities.push_back(e);
}

void EntityManager::clear()
{
  m_entities.clear();
}

EntityManager& EntityManager::getInstance()
{
  static EntityManager instance;
  instance_ = &instance;
  return *instance_;
}
