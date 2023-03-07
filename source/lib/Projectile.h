#pragma once

#include "Unit.h"

struct Projectile {
  Unit* owner;
  raylib::Vector2 velocity;
  raylib::Rectangle rect;

  //We can expand this later with some sort of function object rather than fixed health-damage
  int damage;

  Projectile(Unit* o, Vector2 v, Vector2 p, int d, int width);

  //Returns false when projectile is dead
  bool update();

};