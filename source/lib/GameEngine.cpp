#include "GameEngine.h"
#include "BehaviourNodes.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
  , m_screenWidth(1280)
{
  SetTargetFPS(60);
  Unit u1(Unit::texturePath);
  u1.rect.x = GetRandomValue(0, 1280);
  u1.rect.y = GetRandomValue(0, 720);
  Unit u2(Unit::texturePath);
  u2.rect.x = GetRandomValue(0, 1280);
  u2.rect.y = GetRandomValue(0, 720);
  u2.team = Unit::Computer;
  Unit u3(Unit::texturePath);
  u3.rect.x = GetRandomValue(0, 1280);
  u3.rect.y = GetRandomValue(0, 720);
  Unit u4(Unit::texturePath);
  u4.rect.x = GetRandomValue(0, 1280);
  u4.rect.y = GetRandomValue(0, 720);
  AIManager::getInstance().addUnit(u1);
  /*AIManager::getInstance().addUnit(u2);
  AIManager::getInstance().addUnit(u3);
  AIManager::getInstance().addUnit(u4);*/
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
