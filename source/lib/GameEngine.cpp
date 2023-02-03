#include "GameEngine.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
    , m_screenWidth(1280)
{
  SetTargetFPS(60);
  units.push_back(Unit("./assets/cloak.png"));
  units[0].setMovement(Vector2(0.001f, 0));
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawText("Congrats! You created your first raylib-cpp window!",
           190,
           200,
           20,
           LIGHTGRAY);
  units[0].move();
  units[0].draw();
  EndDrawing();
}

void GameEngine::start() {
  while (!m_window.ShouldClose())  // Detect window close button or ESC key
  {
    updateDrawFrame();
  }
}
