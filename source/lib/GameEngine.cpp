#include "GameEngine.h"
#include "BehaviourNodes.h"
#include "EntityManager.h"

#include <format>

#define SUPPORT_TRACELOG 1

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
  , m_screenWidth(1280)
  , m_background("../../assets/background.png")
{
  SetTargetFPS(60);
  m_activeTeamMap = { {Unit::Team::Player, true}, {Unit::Team::Computer, true} };
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  m_background.Draw(
    raylib::Rectangle(0, 0, m_background.width, m_background.height), //source rectangle, full texture size
    raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight()), //destination rectangle, texture rect
    raylib::Vector2(),
    0
  );

  if (m_running) {
    runSimulation();
  }
  else {
    runPreparation();
  }
  
  EndDrawing();

  if (IsKeyPressed(KEY_SPACE) || !m_activeTeamMap[Unit::Team::Player] || !m_activeTeamMap[Unit::Team::Computer]) {
    if (AIManager::getInstance().getTeamIds(Unit::Team::Player).size() == 0
      || AIManager::getInstance().getTeamIds(Unit::Team::Computer).size() == 0) {
      return;
    }
    
    if (m_running) {
      AIManager::getInstance().reset();

      //Remove out-of-bounds units
      std::vector<UnitId> outOfBoundsUnits;
      for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
        if (!raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight()).CheckCollision(unit->rect)) {
          outOfBoundsUnits.push_back(unit->id);
        }
      }
      for (auto i : outOfBoundsUnits) {
        AIManager::getInstance().removeUnit(*AIManager::getInstance().getUnits()[i]);
      }
    }

    //If either team was defeated, allow 3 seconds to display victory message
    if (!m_activeTeamMap[Unit::Team::Player] || !m_activeTeamMap[Unit::Team::Computer]) {
      WaitTime(3);
    }

    m_running = !m_running;
    m_activeTeamMap = { {Unit::Team::Player, true}, {Unit::Team::Computer, true} };
    EntityManager::getInstance().clear();
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

    //Trigger out-of-bounds 
    if (!raylib::Rectangle(0, 0, GetScreenWidth(), GetScreenHeight()).CheckCollision(unit->rect)) {
      unit->adjustStat(Unit::Stat::Morale, -1, nullptr);
    }

    //If any unit is active, that unit's team is not defeated
    m_activeTeamMap[unit->team] = m_activeTeamMap[unit->team] || unit->active;
  }
  EntityManager::getInstance().tick();
  
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
    0, 0, 16, raylib::Color::Yellow());
  DrawText("Scroll to choose a unit type, and click to place a unit of that type", 
    0, 30, 16, raylib::Color::Yellow());
  DrawText("Press tab to change team between player and computer", 
    0, 60, 16, raylib::Color::Yellow());

  std::string unitTypeName;

  switch (m_currentType) {
  case UnitFactory::UnitType::Melee:
    unitTypeName = "Melee";
    break;
  case UnitFactory::UnitType::Ranged:
    unitTypeName = "Ranged";
    break;
  case UnitFactory::UnitType::Hunter:
    unitTypeName = "Sylvan Hunter";
    break;
  case UnitFactory::UnitType::Sheep:
    unitTypeName = "Sheep";
    break;
  default:
    unitTypeName = "Unknown";
    break;
  }

  //Draw currently selected unit type text
  DrawText(std::format("Selected: {}", unitTypeName).c_str(),
    0, 90, 16, raylib::Color::Yellow());

  DrawText(std::format("Team: {}", m_currentTeam == Unit::Team::Player ? "Player" : "Computer").c_str(),
    0, 120, 16, raylib::Color::Yellow());

  size_t typeInt = static_cast<size_t>(m_currentType);

  typeInt += GetMouseWheelMove();

  typeInt = std::max(std::size_t(0), std::min(typeInt, std::size_t(3)));

  m_currentType = static_cast<UnitFactory::UnitType>(typeInt);

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
