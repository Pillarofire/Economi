#include <string>
#include <vector>
#include "raylib.h"


class Player {
public:
  Vector2 position;
  Vector2 velocity;
  bool active;
private:
  std::string name;
};

class World {
public:
  void Update();
  void Draw();
private:
  // Add your private member variables here
  std::vector<Player> players;
  
};



int main () {
  InitWindow(0,0,"Economi");

  SetTargetFPS(60);
  // SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
  // int screenWidth = GetScreenWidth();
  // int screenHeight = GetScreenHeight();

  // create some entity arrays.


  while (!WindowShouldClose()) {
    // Housekeeping Variables
    const Vector2 mousePos = GetMousePosition();

    // First do the logic update
    

    // Then do the rendering
    BeginDrawing();
    ClearBackground(BLACK);


    EndDrawing();
  }

  CloseWindow();
  return 0; 
}