#pragma once

#include "Vector2.hpp"
#include "Texture.hpp"
#include "Rectangle.hpp"

#include <unordered_map>
#include <string>

class Unit
{
public:
  raylib::Texture texture;
  raylib::Rectangle rect;

  enum Stat
  {
    Health,
    Morale,
    Damage,
    Speed
  };

  enum TeamEnum
  {
    Player,
    Computer
  };

  TeamEnum team;

  std::unordered_map<Stat, int> stats;

  bool active = true;

  protected:
  raylib::Vector2 movement;

  public:
    Unit(const std::string& texturePath);

    //Copy constructor necessary, cannot use std collections (Texture copy assignment operator is deleted)
    Unit(const Unit& u) {
      texture.Load(u.texture.GetData());
      movement = u.movement;
      stats = u.stats;
      active = u.active;
      rect = u.rect;
    }

    //Base classes should always have a virtual deconstructor
    virtual ~Unit() = default;

    //Gets and sets the movement, ensuring it is a unit vector
    const raylib::Vector2& getMovement() const;
    void setMovement(const raylib::Vector2& newMovement);

    raylib::Vector2 getPos() const;

    //Adjusts unit position based on movement vector and time
    void move();

    //Draws the unit on the screen
    void draw();

    //Adusts units stat. Source may be null
    virtual void adjustStat(Stat stat, int amount, Unit* source);

    //Adjusts the stat of another unit
    virtual void adjustTargetStat(Stat stat, int amount, Unit& target);

    //TODO: Consider event system (may be unnecessary)
};