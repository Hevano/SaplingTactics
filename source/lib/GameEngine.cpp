#include "GameEngine.h"

GameEngine::GameEngine() 
  : m_window(1280, 720, "Game Window")
  , m_screenHeight(720)
    , m_screenWidth(1280)
{
  SetTargetFPS(60);
}

void GameEngine::updateDrawFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawText("Congrats! You created your first raylib-cpp window!",
           190,
           200,
           20,
           LIGHTGRAY);
  EndDrawing();
}
