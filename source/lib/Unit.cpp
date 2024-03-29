#include "Unit.h"

#include <format>

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
  return raylib::Vector2(rect.x + rect.width / 2, rect.y + rect.height / 2);
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
  if (selected) {
    raylib::Rectangle highlight(rect);
    highlight.SetSize(highlight.GetSize() * 1.1f);
    highlight.SetPosition(getPos() - (highlight.GetSize() * 0.5f));
    highlight.DrawRoundedLines(0.5f, 32, 2.f, raylib::Color::Yellow());
  }
  texture->Draw(
    raylib::Rectangle(0, 0, texture->width, texture->height), //source rectangle, full texture size
    rect, //destination rectangle, texture rect
    raylib::Vector2(),
    0,
    team == Team::Player ? raylib::Color::Blue() : raylib::Color::Yellow()
  );

  Color c = active ? raylib::Color::Green() : raylib::Color::Red();
  DrawText(std::format("{}/{}", stats[Stat::Health], statsMax[Stat::Health]).c_str(), rect.x, rect.y, 12, c);
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

  if (stats[Stat::Health] <= 0 || stats[Stat::Morale] <= 0) {
    active = false;
  }
}

void Unit::adjustTargetStat(Stat stat, int amount, Unit& target) {
  if (!active) return;
  TraceLog(LOG_DEBUG, "Adusted Stat");
  target.adjustStat(stat, amount, this);
}

void Unit::reset()
{
  for (auto& [stat, value] : stats) {
    value = statsMax[stat];
  }

  for (auto& [type, action] : actions) {
    action.active = false;
  }

  active = true;
  clearMovement();
}
