#pragma once

#include "Unit.h"
#include "Projectile.h"

class RangedUnit : public Unit
{
public:
  using Unit::Unit;
  std::vector<Projectile> projectiles;
  virtual void move();
};