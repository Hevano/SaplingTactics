#pragma once
#include "UnitAction.h"

#include "raylib.hpp"
#include "Vector2.hpp"
#include "Texture.hpp"
#include "Rectangle.hpp"

#include <unordered_map>
#include <string>
#include <memory>

typedef unsigned int UnitId;

class Unit
{
public:
  enum class Stat
  {
    Health,
    Morale,
    Damage,
    Speed
  };

  enum class Team
  {
    Player,
    Computer
  };


  std::shared_ptr<raylib::Texture> texture;
  raylib::Rectangle rect = { 0,0,60,60 };

  UnitId id = 0;
  Team team = Team::Player;

  std::unordered_map<Stat, int> stats;
  std::unordered_map<Stat, int> statsMax;

  bool active = true;
  bool selected = true;

  raylib::Vector2 movement;

  std::unordered_map<UnitAction::ActionType, UnitAction> actions;

public:

  //Gets and sets the movement, ensuring it is a unit vector
  const raylib::Vector2& getMovement() const;
  void setMovement(const raylib::Vector2& newMovement);
  void clearMovement();

  raylib::Vector2 getPos() const;

  //Adjusts unit position based on movement vector and time
  void move();

  //Draws the unit on the screen
  void draw();

  Status takeAction(UnitAction::ActionType type);

  //Adusts units stat. Source may be null
  void adjustStat(Stat stat, int amount, Unit* source);

  //Adjusts the stat of another unit
  void adjustTargetStat(Stat stat, int amount, Unit& target);

  void reset();
};