#pragma once

#include <vector>
#include <memory>


struct Entity {
  bool virtual tick() { return false; };

  virtual ~Entity() = default;
};

class EntityManager {
private:
  std::vector<std::shared_ptr<Entity>> m_entities;
  static inline EntityManager* instance_ = nullptr;
  EntityManager() = default;
  ~EntityManager() = default;

public:
  void tick();
  void addEntity(std::shared_ptr<Entity> e);
  void clear();

  static EntityManager& getInstance();
  
};