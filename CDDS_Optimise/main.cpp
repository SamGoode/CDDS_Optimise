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
#include "int2.h"

bool compareCellHash(int2 a, int2 b) {
    return a.y < b.y;
}

class SpatialHashGrid {
private:
    Vector2 m_pos;
    Vector2 m_size;
    int gridWidth;
    int gridHeight;
    float cellWidth;
    float cellHeight;
    // (particle index, cell index)
    Array<Vector2> positionList;
    Array<int2> hashList;
    Array<int> lookup;

public:
    SpatialHashGrid() {}

    SpatialHashGrid(Vector2 _pos, Vector2 _size, int _gridWidth, int _gridHeight) {
        m_pos = _pos;
        m_size = _size;
        gridWidth = _gridWidth;
        gridHeight = _gridHeight;
        cellWidth = (float)(m_size.x / gridWidth);
        cellHeight = (float)(m_size.y / gridHeight);
    }

    void insertPositions(Array<Vector2> positions) {
        positionList = positions;
        hashList = Array<int2>(positions.getCount());

        for (int i = 0; i < hashList.getCount(); i++) {
            hashList[i] = { i, getCellHash(getCellPos(positions[i])) };
        }
    }

    void sortByCellHash() {
        if (hashList.getCount() == 0) {
            return;
        }

        std::sort(&hashList[0], &hashList[hashList.getCount() - 1], compareCellHash);
    }

    void generateLookup() {
        lookup = Array<int>(gridWidth * gridHeight);
        for (int i = 0; i < lookup.getCount(); i++) {
            lookup[i] = -1;
        }
        
        int currentStart = 0;
        for (unsigned int i = 0; i < (unsigned int)hashList.getCount(); i++) {
            if (i == hashList.getCount() - 1) {
                lookup[hashList[i].y] = currentStart;
                break;
            }

            if (hashList[i].y == hashList[i + 1].y) {
                continue;
            }

            lookup[hashList[i].y] = currentStart;
            currentStart = i + 1;
        }
    }

    Array<int> findNearby(int2 cellPos) {
        Array<int> positionIndexes;

        for (int i = 0; i < 3; i++) {
            for (int n = 0; n < 3; n++) {
                int2 queryPos = { cellPos.x + i - 1, cellPos.y + n - 1 };
                if (queryPos.x < 0 || queryPos.x >= gridWidth || queryPos.y < 0 || queryPos.y >= gridHeight) {
                    continue;
                }

                int cellHash = getCellHash(queryPos);
                int startIndex = lookup[cellHash];

                for (int i = startIndex; i < hashList.getCount() && hashList[i].y == cellHash; i++) {
                    positionIndexes.append(hashList[startIndex].x);
                }
            }
        }

        return positionIndexes;
    }

    bool isValidPos(Vector2 pos) {
        return (pos.x >= m_pos.x && pos.x < m_pos.x + m_size.x && pos.y >= m_pos.y && pos.y < m_pos.y + m_size.y);
    }

    int2 getCellPos(Vector2 pos) {
        Vector2 adjusted = Vector2Subtract(pos, m_pos);
        int2 cellPos = { (int)(adjusted.x / cellWidth), (int)(adjusted.y / cellHeight) };
        if (cellPos.x < 0) {
            cellPos.x = 0;
        }
        else if (cellPos.x >= gridWidth) {
            cellPos.x = gridWidth - 1;
        }

        if (cellPos.y < 0) {
            cellPos.y = 0;
        }
        else if (cellPos.y >= gridWidth) {
            cellPos.y = gridWidth - 1;
        }

        return cellPos;
    }

    int getCellHash(int2 cellPos) {
        return (cellPos.x + cellPos.y * gridWidth);
    }

    void draw() {
        sortByCellHash();
        generateLookup();
        Vector2 pos = GetMousePosition();
        //Vector2 pos = positionList[0];

        int2 cellPos = getCellPos(pos);
        if (!isValidPos(pos)) {
            cellPos = { 1, 1 };
        }
        int x = cellPos.x;
        int y = cellPos.y;
        DrawRectangle(m_pos.x + (x - 1) * cellWidth, m_pos.y + (y - 1) * cellHeight, cellWidth * 3, cellHeight * 3, ORANGE);
        DrawRectangle(m_pos.x + x * cellWidth, m_pos.y + y * cellHeight, cellWidth, cellHeight, RED);

        for (int i = 0; i < gridWidth; i++) {
            for (int n = 0; n < gridHeight; n++) {
                DrawRectangleLines(m_pos.x + i * cellWidth, m_pos.y + n * cellHeight, cellWidth, cellHeight, BLACK);
            }
        }

        DrawRectangleLines(m_pos.x, m_pos.y, m_size.x, m_size.y, BLUE);

        Array<int> indexes = findNearby(cellPos);

        for (int i = 0; i < positionList.getCount(); i++) {
            DrawCircle(positionList[i].x, positionList[i].y, 15, GREEN);
        }

        for (int i = 0; i < indexes.getCount(); i++) {
            DrawCircle(positionList[indexes[i]].x, positionList[indexes[i]].y, 15, RED);
        }

        DrawCircle(pos.x, pos.y, 15, BLUE);

        std::string cellID = "Cell ID: " + std::to_string(getCellHash(cellPos));
        DrawText(cellID.c_str(), 10, 100, 20, PURPLE);
        std::string nearby = "Within bounds: " + std::to_string(indexes.getCount());
        DrawText(nearby.c_str(), 10, 140, 20, PURPLE);

        std::string valid = isValidPos(GetMousePosition()) ? "true" : "false";
        valid = "Valid location: " + valid;
        DrawText(valid.c_str(), 10, 160, 20, PURPLE);
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

    SpatialHashGrid grid({ 0, 0 }, { (float)screenWidth, (float)screenHeight }, 16, 8);

    // create some critters 
    Array<Critter> critters(50);
    ObjectPool critterPool(critters.getCount());

    const int MAX_VELOCITY = 80;

    for (int i = 0; i < critters.getCount(); i++) {
        // create a random position vector for the location
        Vector2 position = { (float)(5 + rand() % (screenWidth - 10)), (float)(5 + (rand() % screenHeight - 10)) };

        // create a random direction vector for the velocity
        Vector2 velocity = { -100 + (rand() % 200), -100 + (rand() % 200) };
        // normalize and scale max velocity
        velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

        // create a critter in a random location
        critters[i] = Critter(position, velocity, &textureMap);
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
            //int n = critterPool[i];
            Critter& critter = critters[critterPool[i]];
            critter.Update(delta);

            // check each critter against screen bounds
            if (critter.GetX() < 0) {
                critter.SetX(0);
                critter.SetVelocity(Vector2{ -critter.GetVelocity().x, critter.GetVelocity().y });
            }
            if (critter.GetX() > screenWidth) {
                critter.SetX(screenWidth);
                critter.SetVelocity(Vector2{ -critter.GetVelocity().x, critter.GetVelocity().y });
            }
            if (critter.GetY() < 0) {
                critter.SetY(0);
                critter.SetVelocity(Vector2{ critter.GetVelocity().x, -critter.GetVelocity().y });
            }
            if (critter.GetY() > screenHeight) {
                critter.SetY(screenHeight);
                critter.SetVelocity(Vector2{ critter.GetVelocity().x, -critter.GetVelocity().y });
            }

            // kill any critter touching the destroyer
            // simple circle-to-circle collision check
            float dist = Vector2Distance(critter.GetPosition(), destroyer.GetPosition());
            if (dist < critter.GetRadius() + destroyer.GetRadius()) {
                critterPool.unloadObject(i);
            }
        }

        Array<Vector2> boop;
        for (int i = 0; i < critterPool.getActive(); i++) {
            Vector2 position = critters[critterPool[i]].GetPosition();
            if (grid.isValidPos(position)) {
                boop.append(position);
            }
        }
        grid.insertPositions(boop);
                
        // check for critter-on-critter collisions
        for (int i = 0; i < critterPool.getActive(); i++) {
            for (int j = 0; j < critterPool.getActive(); j++){
                Critter& critterA = critters[critterPool[i]];
                Critter& critterB = critters[critterPool[j]];
                if (i == j || critterA.IsDirty()) {
                    continue;// note: the other critter (j) could be dirty - that's OK
                }

                // check every critter against every other critter
                float dist = Vector2Distance(critterA.GetPosition(), critterB.GetPosition());
                if (dist < critterA.GetRadius() + critterB.GetRadius()) {
                    // collision!
                    // do math to get critters bouncing
                    Vector2 normal = Vector2Normalize( Vector2Subtract(critterB.GetPosition(), critterA.GetPosition()));

                    // not even close to real physics, but fine for our needs
                    critterA.SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
                    // set the critter to *dirty* so we know not to process any more collisions on it
                    critterA.SetDirty();

                    // we still want to check for collisions in the case where 1 critter is dirty - so we need a check 
                    // to make sure the other critter is clean before we do the collision response
                    if (!critterB.IsDirty()) {
                        critterB.SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
                        critterB.SetDirty();
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

                critters[critterPool.loadObject()].InitVecs(pos, Vector2Scale(normal, -MAX_VELOCITY));
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
            critters[critterPool[i]].Draw();
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