#include "UnitFactory.h"
#include "AIManager.h"

Unit& UnitFactory::makeUnit(UnitType type, Unit::Team team)
{
  auto typeIndex = static_cast<size_t>(type);
  auto& unit = AIManager::getInstance().addUnit(++idCount, team, unitTreePaths[typeIndex]);
  unit.id = idCount;
  unit.team = team;

  if (!m_unitTextures.contains(type)) {
    m_unitTextures.emplace(type, std::make_shared<raylib::Texture>(raylib::Texture(unitTexturePaths[typeIndex])));
  }

  unit.texture = m_unitTextures[type];

  unit.stats = unitStats[typeIndex];
  unit.statsMax = unitStats[typeIndex];

  switch (type) {
  case UnitType::Melee:
    makeMelee(type, unit);
    break;
  case UnitType::Ranged:
    makeRanged(type, unit);
    break;
  }

  return unit;
}

void UnitFactory::makeRanged(UnitType type, Unit& unit)
{
  unit.actions.emplace(UnitAction::RangedAttack, UnitAction());
  unit.actions[UnitAction::RangedAttack].effect = [](Unit& u, UnitAction& action)
  {
    //initialize the projectile
    if (!action.active) {
      auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
      auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];

      action.data.push_back(std::make_any<raylib::Vector2>(u.getPos()));
      action.data.push_back(std::make_any<raylib::Vector2>((targetUnit->getPos() - u.getPos()).Normalize()));
    }

    auto projectilePos = std::any_cast<raylib::Vector2>(action.data[0]);
    auto projectileVelocity = std::any_cast<raylib::Vector2>(action.data[1]);

    projectilePos += projectileVelocity * u.statsMax[Unit::Stat::Speed] * GetFrameTime() * 1.5f;

    action.data[0] = std::make_any<raylib::Vector2>(projectilePos);

    projectilePos.DrawCircle(10, raylib::Color::Red());

    for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
      if (id != u.id && unit->rect.CheckCollision(raylib::Rectangle(projectilePos, raylib::Vector2(20,20)))) {
        u.adjustTargetStat(Unit::Stat::Health, -u.stats[Unit::Stat::Damage], *unit);
        return Status::Success;
      }
    }

    //If not onscreen delete the projectile
    if (!raylib::Rectangle(raylib::Vector2(0.f), raylib::Vector2(GetScreenWidth(), GetScreenHeight())).CheckCollision(projectilePos)) {
      return Status::Failure;
    }

    
    return Status::Running;
  };
}

void UnitFactory::makeMelee(UnitType type, Unit& unit)
{
  unit.actions.emplace(UnitAction::MeleeAttack, UnitAction());
  unit.actions[UnitAction::MeleeAttack].effect = [](Unit& u, UnitAction& action)
  {
    auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
    auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
    if (u.rect.GetPosition().Distance(targetUnit->rect.GetPosition()) > 50.f) {
      return Status::Failure;
    }

    u.adjustTargetStat(Unit::Stat::Health, -u.stats[Unit::Stat::Damage], *targetUnit);
    return Status::Success;
  };
}


