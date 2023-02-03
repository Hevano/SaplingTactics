#include "Unit.h"

Unit::Unit(const std::string& texturePath) 
  : texture(texturePath)
  , rect(0,0,100, 100)
  , team(Unit::TeamEnum::Player)
{
  stats[Stat::Speed] = 1;
  stats[Stat::Health] = 1;
  stats[Stat::Damage] = 1;
  stats[Stat::Morale] = 1;
}

const raylib::Vector2& Unit::getMovement() const
{
  return movement;
}

void Unit::setMovement(const raylib::Vector2& newMovement)
{
  movement = newMovement.Normalize();
}

raylib::Vector2 Unit::getPos() const
{
  return raylib::Vector2(rect.x, rect.y);
}

void Unit::move() {
  if (!active) return;
  Vector2 move = movement * stats[Stat::Speed] * GetFrameTime();

}

void Unit::draw() {
  texture.Draw(
    raylib::Rectangle(0,0, texture.width, texture.height), //source rectangle, full texture size
    rect, //destination rectangle, texture rect
    raylib::Vector2(),
    0
  );
}

void Unit::adjustStat(Stat stat, int amount, Unit* source) {
  stats[stat] += amount;

  if (stats[Stat::Health] <= 0) {
    active = false;
  }
}

void Unit::adjustTargetStat(Stat stat, int amount, Unit& target) {
  if (!active) return;
  target.adjustStat(stat, amount, this);
}
