#include "GameEngine.h"
#include "BehaviourNodes.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
  , m_screenWidth(1280)
{
  SetTargetFPS(60);
  auto& u1 = m_unitFactory.makeUnit(UnitFactory::UnitType::Ranged, Unit::Team::Computer);
  u1.rect.x = 1280 / 2; //GetRandomValue(0, 1280);
  u1.rect.y = 720 / 2; //GetRandomValue(0, 720);
  u1.setMovement(u1.getPos());
  auto& u2 = m_unitFactory.makeUnit(UnitFactory::UnitType::Melee, Unit::Team::Player);
  u2.rect.x = GetRandomValue(0, 1280);
  u2.rect.y = GetRandomValue(0, 720);
  auto& u3 = m_unitFactory.makeUnit(UnitFactory::UnitType::Melee, Unit::Team::Player);
  u3.rect.x = GetRandomValue(0, 1280);
  u3.rect.y = GetRandomValue(0, 720);
  auto& u4 = m_unitFactory.makeUnit(UnitFactory::UnitType::Melee, Unit::Team::Player);
  u4.rect.x = GetRandomValue(0, 1280);
  u4.rect.y = GetRandomValue(0, 720);
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  AIManager::getInstance().tick();
  for (auto& [id, unit] : AIManager::getInstance().getUnits()) {
    unit->move();
    unit->draw();
  }
  EndDrawing();
}

void GameEngine::start() {
  while (!m_window.ShouldClose())  // Detect window close button or ESC key
  {
    updateDrawFrame();
  }
}
