#pragma once

#include "Unit.h"

#include <vector>
#include <string>
#include <memory>

class UnitFactory {
public:
enum class UnitType : size_t {
  Melee = 0,
  Ranged = 1
};

private:

static inline UnitId idCount = 0;

const inline static std::vector<std::string> unitTexturePaths = {
  "./assets/cloak.png",
  "./assets/cloak.png",
};

const inline static std::vector<std::string> unitTreePaths = {
  "./assets/wanderDesignExport.json",
  "./assets/turretDesignExport.json",
};

const inline static std::vector<std::unordered_map<Unit::Stat, int>> unitStats = {
  {{Unit::Stat::Speed, 100},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 100},{Unit::Stat::Health, 100}},
  {{Unit::Stat::Speed, 100},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 100},{Unit::Stat::Health, 100}},
};

  std::unordered_map<UnitType, std::shared_ptr<raylib::Texture>> m_unitTextures;


public:
  Unit& makeUnit(UnitType type, Unit::Team team);

private:
  void makeMelee(UnitType type, Unit& unit);
  void makeRanged(UnitType type, Unit& unit);

};

