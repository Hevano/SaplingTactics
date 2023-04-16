#include "UnitFactory.h"
#include "AIManager.h"
#include "EntityManager.h"
#include "ProjectileEntity.h"

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
    makeMelee(unit);
    break;
  case UnitType::Ranged:
    makeRanged(unit);
    break;
  case UnitType::Hunter:
    makeHunter(unit);
    break;
  }

  return unit;
}

void UnitFactory::makeRanged(Unit& unit)
{
  unit.actions.emplace(UnitAction::RangedAttack, UnitAction());
  unit.actions[UnitAction::RangedAttack].effect = [](Unit& u, UnitAction& action)
  {
    if (!action.active) {
      auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
      auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
      std::shared_ptr<ProjectileEntity> p(new ProjectileEntity(
        u.getPos(),
        u.stats[Unit::Stat::Speed] * 1.5f,
        targetUnit->getPos(),
        &u,
        u.stats[Unit::Stat::Damage]));
      EntityManager::getInstance().addEntity(p);
      action.data.push_back(std::make_any<float>(GetTime()));
    }

    if (std::any_cast<float>(action.data[0]) + 1.f > GetTime()) {
      return Status::Running;
    }

    return Status::Success;
  };
}

void UnitFactory::makeHunter(Unit& unit)
{
  unit.actions.emplace(UnitAction::MeleeAttack, UnitAction());
  unit.actions[UnitAction::MeleeAttack].effect = [](Unit& u, UnitAction& action)
  {
    if (!action.active) {
      auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
      auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
      if (u.rect.GetPosition().Distance(targetUnit->rect.GetPosition()) > 120.f) {
        return Status::Failure;
      }
      u.adjustTargetStat(Unit::Stat::Health, -u.stats[Unit::Stat::Damage] * 0.75, *targetUnit);

      action.data.push_back(std::make_any<float>(GetTime()));
    }

    if (std::any_cast<float>(action.data[0]) + 1.f > GetTime()) {
      return Status::Running;
    }
    
    return Status::Success;
  };

  unit.actions.emplace(UnitAction::RangedAttack, UnitAction());
  unit.actions[UnitAction::RangedAttack].effect = [](Unit& u, UnitAction& action)
  {
    if (!action.active) {
      auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
      auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
      std::shared_ptr<ProjectileEntity> p(new ProjectileEntity(
        u.getPos(),
        u.stats[Unit::Stat::Speed] * 1.5f,
        targetUnit->getPos(),
        &u,
        u.stats[Unit::Stat::Damage] * 0.25f));
      EntityManager::getInstance().addEntity(p);
      action.data.push_back(std::make_any<float>(GetTime()));
    }

    if (std::any_cast<float>(action.data[0]) + 1.f > GetTime()) {
      return Status::Running;
    }

    return Status::Success;
  };
}

void UnitFactory::makeMelee(Unit& unit)
{
  unit.actions.emplace(UnitAction::MeleeAttack, UnitAction());
  unit.actions[UnitAction::MeleeAttack].effect = [](Unit& u, UnitAction& action)
  {
    if (!action.active) {
      auto attackTargetId = std::any_cast<UnitId>(AIManager::getInstance().getUnitBlackboard(u.id).at("AttackTarget"));
      auto targetUnit = AIManager::getInstance().getUnits()[attackTargetId];
      if (u.rect.GetPosition().Distance(targetUnit->rect.GetPosition()) > 120.f) {
        return Status::Failure;
      }
      u.adjustTargetStat(Unit::Stat::Health, -u.stats[Unit::Stat::Damage], *targetUnit);

      action.data.push_back(std::make_any<float>(GetTime()));
    }

    if (std::any_cast<float>(action.data[0]) + 1.f > GetTime()) {
      return Status::Running;
    }

    return Status::Success;
  };
}


