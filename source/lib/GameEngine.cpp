#include "GameEngine.h"
#include "BehaviourNodes.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
  , m_screenWidth(1280)
{
  SetTargetFPS(60);
  Unit u(Unit::texturePath);
  AIManager::getInstance().addUnit(u);
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawText("Congrats! You created your first raylib-cpp window!",
           190,
           200,
           20,
           LIGHTGRAY);
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
