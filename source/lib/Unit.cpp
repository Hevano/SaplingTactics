#include "Unit.h"

Unit::Unit(const std::string& texturePath) 
  : texture(texturePath)
  , rect(0,0,100, 100)
  , team(Unit::TeamEnum::Player)
  , id(idCount_++) //increment id with each new unit created
{
  stats[Stat::Speed] = 100;
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
  movement = newMovement;
}

void Unit::clearMovement()
{
  movement = rect.GetPosition();
}

raylib::Vector2 Unit::getPos() const
{
  return raylib::Vector2(rect.x, rect.y);
}

void Unit::move() {
  if (!active) return;
  Vector2 move = Vector2MoveTowards(rect.GetPosition(), movement, stats[Stat::Speed] * GetFrameTime());
  rect.SetPosition(move);
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
  TraceLog(LOG_DEBUG, "Adusted Stat");
  target.adjustStat(stat, amount, this);
}
