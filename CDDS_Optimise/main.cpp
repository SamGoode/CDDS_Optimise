/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <random>
#include <time.h>
#include <string>
#include "TextureMap.h"
#include "ObjectPool.h"
#include "Array.h"
#include "Critter.h"
#include "Destroyer.h"

Array<Vector2> positions(10);

class SpatialHashGrid {
private:
    Vector2 pos;
    int width;
    int height;
    int gridWidth;
    int gridHeight;
    int cellWidth;
    int cellHeight;
    struct GridPos { int x, y; };

public:
    SpatialHashGrid() {}

    SpatialHashGrid(Vector2 _pos, int _width, int _height, int _gridWidth, int _gridHeight) {
        pos = _pos;
        width = _width;
        height = _height;
        gridWidth = _gridWidth;
        gridHeight = _gridHeight;
        cellWidth = width / gridWidth;
        cellHeight = height / gridHeight;
    }

    GridPos calculateGridPos(Vector2 query) {
        Vector2 adjusted = Vector2Subtract(query, pos);
        return { (int)adjusted.x / cellWidth, (int)adjusted.y / cellHeight };
    }

    int generateHashKey(GridPos gridPos) {
        return (gridPos.x * 13 + gridPos.y * 7) % (gridWidth * gridHeight);
    }

    void insertPositions(Array<Vector2> positions) {
        Array<int> hashList(positions.getCount());

        for (int i = 0; i < positions.getCount(); i++) {
            hashList[i] = generateHashKey(calculateGridPos(positions[i]));
        }
    }

    void draw() {
        GridPos gridPos = calculateGridPos(GetMousePosition());
        int x = gridPos.x;
        int y = gridPos.y;
        DrawRectangle(pos.x + (x - 1) * cellWidth, pos.y + (y - 1) * cellHeight, cellWidth * 3, cellHeight * 3, RED);
        DrawRectangle(pos.x + x * cellWidth, pos.y + y * cellHeight, cellWidth, cellHeight, BLUE);

        for (int i = 0; i < gridWidth; i++) {
            for (int n = 0; n < gridHeight; n++) {
                DrawRectangleLines(pos.x + i * cellWidth, pos.y + n * cellHeight, cellWidth, cellHeight, BLACK);
            }
        }

        DrawText(std::to_string(generateHashKey(gridPos)).c_str(), 10, 100, 20, RED);
    }
};


int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    TextureMap textureMap(2);
    textureMap.assign(0, LoadTexture("res/10.png"));
    textureMap.assign(1, LoadTexture("res/9.png"));

    //SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    srand(time(NULL));

    SpatialHashGrid grid({ 0, 0 }, screenWidth, screenHeight, 16, 8);

    // create some critters 
    ObjectPool<Critter> critterPool(50);

    const int MAX_VELOCITY = 80;

    for (int i = 0; i < critterPool.getTotal(); i++) {
        // create a random position vector for the location
        Vector2 position = { (float)(5 + rand() % (screenWidth - 10)), (float)(5 + (rand() % screenHeight - 10)) };

        // create a random direction vector for the velocity
        Vector2 velocity = { -100 + (rand() % 200), -100 + (rand() % 200) };
        // normalize and scale max velocity
        velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

        // create a critter in a random location
        critterPool[i] = Critter(position, velocity, &textureMap);
    }
    
    // create a position vector for the centre of the screen
    Vector2 position = { (float)(screenWidth >> 1), (float)(screenHeight >> 1) };

    // create a random direction vector for the velocity
    Vector2 velocity = { -100 + (rand() % 200), -100 + (rand() % 200) };
    // normalize and scale by max velocity
    velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

    //create a destroyer at centre of screen
    Destroyer destroyer = Destroyer(position, velocity, &textureMap);

    float timer = 1;
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        float delta = GetFrameTime();

        //update the destroyer
        destroyer.Update(delta);
        // check destroyer against screen bounds
        if (destroyer.GetX() < 0) {
            destroyer.SetX(0);
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetX() > screenWidth) {
            destroyer.SetX(screenWidth);
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() < 0) {
            destroyer.SetY(0);
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() > screenHeight) {
            destroyer.SetY(screenHeight);
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }

        // update the critters
        // (dirty flags will be cleared during update)
        for (int i = 0; i < critterPool.getActive(); i++) {
            critterPool[i].Update(delta);

            // check each critter against screen bounds
            if (critterPool[i].GetX() < 0) {
                critterPool[i].SetX(0);
                critterPool[i].SetVelocity(Vector2{ -critterPool[i].GetVelocity().x, critterPool[i].GetVelocity().y });
            }
            if (critterPool[i].GetX() > screenWidth) {
                critterPool[i].SetX(screenWidth);
                critterPool[i].SetVelocity(Vector2{ -critterPool[i].GetVelocity().x, critterPool[i].GetVelocity().y });
            }
            if (critterPool[i].GetY() < 0) {
                critterPool[i].SetY(0);
                critterPool[i].SetVelocity(Vector2{ critterPool[i].GetVelocity().x, -critterPool[i].GetVelocity().y });
            }
            if (critterPool[i].GetY() > screenHeight) {
                critterPool[i].SetY(screenHeight);
                critterPool[i].SetVelocity(Vector2{ critterPool[i].GetVelocity().x, -critterPool[i].GetVelocity().y });
            }

            // kill any critter touching the destroyer
            // simple circle-to-circle collision check
            float dist = Vector2Distance(critterPool[i].GetPosition(), destroyer.GetPosition());
            if (dist < critterPool[i].GetRadius() + destroyer.GetRadius()) {
                critterPool.unloadObject(i);
            }
        }
                
        // check for critter-on-critter collisions
        for (int i = 0; i < critterPool.getActive(); i++) {
            for (int j = 0; j < critterPool.getActive(); j++){
                if (i == j || critterPool[i].IsDirty()) {
                    continue;// note: the other critter (j) could be dirty - that's OK
                }

                // check every critter against every other critter
                float dist = Vector2Distance(critterPool[i].GetPosition(), critterPool[j].GetPosition());
                if (dist < critterPool[i].GetRadius() + critterPool[j].GetRadius()) {
                    // collision!
                    // do math to get critters bouncing
                    Vector2 normal = Vector2Normalize( Vector2Subtract(critterPool[j].GetPosition(), critterPool[i].GetPosition()));

                    // not even close to real physics, but fine for our needs
                    critterPool[i].SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
                    // set the critter to *dirty* so we know not to process any more collisions on it
                    critterPool[i].SetDirty();

                    // we still want to check for collisions in the case where 1 critter is dirty - so we need a check 
                    // to make sure the other critter is clean before we do the collision response
                    if (!critterPool[j].IsDirty()) {
                        critterPool[j].SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
                        critterPool[j].SetDirty();
                    }
                    break;
                }
            }
        }

        timer -= delta;
        if (timer <= 0) {
            timer = 1;

            if (critterPool.getInactive() > 0) {
                // get a position behind the destroyer, and far enough away that the critter won't bump into it again
                Vector2 normal = Vector2Normalize(destroyer.GetVelocity());
                Vector2 pos = destroyer.GetPosition();
                pos = Vector2Add(pos, Vector2Scale(normal, -70));

                critterPool.loadObject().InitVecs(pos, Vector2Scale(normal, -MAX_VELOCITY));
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        grid.draw();

        // draw the destroyer
        destroyer.Draw();

        // draw the critters
        for (int i = 0; i < critterPool.getActive(); i++) {
            critterPool[i].Draw();
        }
        
        DrawFPS(10, 10);
        std::string debug = "Critters total: " + std::to_string(critterPool.getTotal()) + "\nCritters alive: " + std::to_string(critterPool.getActive()) + "\nCritters dead: " + std::to_string(critterPool.getInactive());
        DrawText(debug.c_str(), 10, 32, 16, BLUE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i = 0; i < textureMap.getCount(); i++) {
        UnloadTexture(textureMap[i]);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}