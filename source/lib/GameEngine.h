#pragma once
#include "Unit.h"

#include "raylib-cpp.hpp"

#include <vector>

class GameEngine
{
private:
  raylib::Window m_window;
  int m_screenWidth, m_screenHeight;


public:
  GameEngine();
  void updateDrawFrame();
  void start();

  std::vector<Unit> units;
};