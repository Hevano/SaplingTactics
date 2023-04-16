#include "ProjectileEntity.h"
#include "raylib.hpp"
#include "AIManager.h"



ProjectileEntity::ProjectileEntity(raylib::Vector2 p, float speed, raylib::Vector2 target, Unit* owner, int damage)
  : position(p)
  , owner(owner)
  , damage(damage)
{
  velocity = (target - position).Normalize() * speed;
}

bool ProjectileEntity::tick()
{
  position += velocity * GetFrameTime();
  position.DrawCircle(10, raylib::Color::Red());

  auto& u = *owner;

  for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
    if (id != u.id && unit->rect.CheckCollision(raylib::Rectangle(position, raylib::Vector2(20, 20)))) {
      u.adjustTargetStat(Unit::Stat::Health, -u.stats[Unit::Stat::Damage], *unit);
      return false;
    }
  }

  //If not onscreen delete the projectile
  if (!raylib::Rectangle(raylib::Vector2(0.f), raylib::Vector2(GetScreenWidth(), GetScreenHeight())).CheckCollision(position)) {
    return false;
  }
  return true;
}
