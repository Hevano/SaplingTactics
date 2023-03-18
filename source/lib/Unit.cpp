#include "Unit.h"

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
  for (auto& [type, action] : actions) {
    if (action.status == Status::Running) {
      action.tick(*this);
    }
  }
}

void Unit::draw() {
  texture->Draw(
    raylib::Rectangle(0,0, texture->width, texture->height), //source rectangle, full texture size
    rect, //destination rectangle, texture rect
    raylib::Vector2(),
    0
  );
}

Status Unit::takeAction(UnitAction::ActionType type)
{
  if (!actions[type].active) {
    actions[type].tick(*this);
    actions[type].active = true;
  }
  else if (actions[type].status != Status::Running) {
    actions[type].active = false;
  }
  return actions[type].status;
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
