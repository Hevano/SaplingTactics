#pragma once
#include "UnitFactory.h"

#include "raylib-cpp.hpp"

#include <vector>

class GameEngine
{
private:
  raylib::Window m_window;
  int m_screenWidth, m_screenHeight;
  UnitFactory m_unitFactory;


public:
  GameEngine();
  void updateDrawFrame();
  void start();
};