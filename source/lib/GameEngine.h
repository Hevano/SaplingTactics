#pragma once
#include "raylib-cpp.hpp"

class GameEngine
{
public:
  GameEngine();
  void updateDrawFrame();

private:
  raylib::Window m_window;
  int m_screenWidth, m_screenHeight;
};