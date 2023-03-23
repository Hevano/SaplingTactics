#pragma once

#include "Unit.h"

#include <vector>
#include <string>
#include <memory>

class UnitFactory {
public:
enum class UnitType : size_t {
  Melee = 0,
  Ranged = 1,
  Hunter = 2,
  Sheep = 3,
};

private:

static inline UnitId idCount = 0;

const inline static std::vector<std::string> unitTexturePaths = {
  "./assets/cloak.png",
  "./assets/cloak2.png",
  "./assets/cloak2.png",
  "./assets/cloak2.png",
};

const inline static std::vector<std::string> unitTreePaths = {
  "./assets/wanderDesignExport.json",
  "./assets/wizardDesignExport.json",
  "./assets/hunterDesignExport.json",
  "./assets/wanderDesignExport.json",
};

const inline static std::vector<std::unordered_map<Unit::Stat, int>> unitStats = {
  {{Unit::Stat::Speed, 100},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 100},{Unit::Stat::Health, 100}},
  {{Unit::Stat::Speed, 75},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 100},{Unit::Stat::Health, 50}},
  {{Unit::Stat::Speed, 125},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 125},{Unit::Stat::Health, 75}},
  {{Unit::Stat::Speed, 100},{Unit::Stat::Morale, 100},{Unit::Stat::Damage, 200},{Unit::Stat::Health, 200}},
};

  std::unordered_map<UnitType, std::shared_ptr<raylib::Texture>> m_unitTextures;


public:
  Unit& makeUnit(UnitType type, Unit::Team team);

private:
  void makeMelee(Unit& unit);
  void makeRanged(Unit& unit);
  void makeHunter(Unit& unit);

};

