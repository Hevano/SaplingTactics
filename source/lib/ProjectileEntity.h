#pragma once

#include "EntityManager.h"
#include "Unit.h"


#include "raylib-cpp.hpp"


struct ProjectileEntity : Entity {

  raylib::Vector2 position;
  raylib::Vector2 velocity;
  Unit* owner;
  int damage;


  ProjectileEntity(raylib::Vector2 p, float speed, raylib::Vector2 target, Unit* owner, int damage);

  bool tick() override;
};