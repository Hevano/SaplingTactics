#include "GameEngine.h"
#include "BehaviourNodes.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
  , m_screenWidth(1280)
{
  SetTargetFPS(60);
  m_activeTeamMap = { {Unit::Team::Player, true}, {Unit::Team::Computer, true} };
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  if (m_running) {
    runSimulation();
  }
  else {
    runPreparation();
  }
  
  EndDrawing();

  if (IsKeyPressed(KEY_SPACE) || !m_activeTeamMap[Unit::Team::Player] || !m_activeTeamMap[Unit::Team::Computer]) {
    if (m_running) {
      AIManager::getInstance().reset();
    }

    //If either team was defeated, allow 3 seconds to display victory message
    if (!m_activeTeamMap[Unit::Team::Player] || !m_activeTeamMap[Unit::Team::Computer]) {
      WaitTime(3);
    }

    m_running = !m_running;
    m_activeTeamMap = { {Unit::Team::Player, true}, {Unit::Team::Computer, true} };
  }
}

void GameEngine::start() {
  while (!m_window.ShouldClose())  // Detect window close button or ESC key
  {
    updateDrawFrame();
  }
}

void GameEngine::runSimulation()
{
  m_activeTeamMap[Unit::Team::Player] = false;
  m_activeTeamMap[Unit::Team::Computer] = false;

  auto& ai = AIManager::getInstance();
  ai.tick();
  for (auto& [id, unit] : ai.getUnits()) {
    unit->move();
    unit->draw();
    //If any unit is active, that unit's team is not defeated
    m_activeTeamMap[unit->team] = m_activeTeamMap[unit->team] || unit->active;
  }
  
  if (!m_activeTeamMap[Unit::Team::Player]) {
    DrawText("Computer Team Wins!",
      GetScreenWidth() / 2, GetScreenHeight() / 2, 30, raylib::Color::Black());
  }
  else if (!m_activeTeamMap[Unit::Team::Computer]) {
    DrawText("Player Team Wins!",
      GetScreenWidth() / 2, GetScreenHeight() / 2, 30, raylib::Color::Black());
  }
}

void GameEngine::runPreparation()
{
  //Draw Existing Units
  for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
    unit->draw();
  }


  //Draw Instructions Text
  DrawText("Press [SPACE] to start or reset the simulation", 
    0, 0, 16, raylib::Color::Black());
  DrawText("Press the Number keys (1-2) to choose a unit type, and click to place a unit", 
    0, 30, 16, raylib::Color::Black());
  DrawText("Press tab to change team between player and computer", 
    0, 60, 16, raylib::Color::Black());

  //Draw currently selected unit type text
  DrawText(std::format("Selected: {}", m_currentType == UnitFactory::UnitType::Ranged ? "Ranged" : "Melee").c_str(), 
    0, 90, 16, raylib::Color::Black());

  DrawText(std::format("Team: {}", m_currentTeam == Unit::Team::Player ? "Player" : "Computer").c_str(),
    0, 120, 16, raylib::Color::Black());

  //Poll new unit selected
  if (IsKeyPressed(KEY_ONE)) {
    m_currentType = UnitFactory::UnitType::Melee;
  } else if (IsKeyPressed(KEY_TWO)) {
    m_currentType = UnitFactory::UnitType::Ranged;
  }

  //Poll Team changed
  if (IsKeyPressed(KEY_TAB)) {
    m_currentTeam = (m_currentTeam == Unit::Team::Player ? Unit::Team::Computer : Unit::Team::Player);
  }
  
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //Poll unit placed
    auto& newUnit = m_unitFactory.makeUnit(m_currentType, m_currentTeam);
    newUnit.rect.SetPosition(GetMousePosition());
    newUnit.clearMovement();
  }
  else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) { //Poll unit deleted
    Unit* unitToDelete = nullptr;
    for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
      if (unit->rect.CheckCollision(GetMousePosition())) {
        unitToDelete = unit.get();
      }
    }
    if (unitToDelete != nullptr) {
      AIManager::getInstance().removeUnit(*unitToDelete);
    }
  }
}
