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
  bool m_running = false;

  std::unordered_map<Unit::Team, bool> m_activeTeamMap;
  UnitFactory::UnitType m_currentType = UnitFactory::UnitType::Ranged;
  Unit::Team m_currentTeam = Unit::Team::Player;


public:
  GameEngine();
  void updateDrawFrame();
  void start();

private:
  void runSimulation();
  void runPreparation();
};