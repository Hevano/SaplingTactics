#include "Projectile.h"
#include "AIManager.h"

Projectile::Projectile(Unit* o, Vector2 v, Vector2 p, int d, int width)
    : owner(o)
    , velocity(v)
    , rect(Vector2(width,width))
    , damage(d)
{
  rect.SetPosition(width * 0.5 + p.x, width * 0.5 + p.y);
}

bool Projectile::update() {
  rect.GetPosition().Add(velocity * GetTime());
  auto enemyTeam = (owner->team == Unit::Player) ? Unit::Computer : Unit::Player;
  auto& units = AIManager::getInstance().getUnits();
  for (auto& id : AIManager::getInstance().getTeamIds(enemyTeam)) {
    auto& unit = units[id];
    if (unit->rect.CheckCollision(rect)) {
      unit->adjustStat(Unit::Health, damage, owner);
      return false;
      delete this;
    }
  }
  //Projectile is out of bounds
  if (!raylib::Rectangle(Vector2(2000, 2000)).CheckCollision(rect)) {
    delete this;
    return false;
  }

  rect.DrawRounded(1.f, 8, RED);
  return true;
}
