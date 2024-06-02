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
#include "SpatialHashGrid.h"

int main(int argc, char* argv[]) {
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    TextureMap textureMap(2);
    textureMap.assign(0, LoadTexture("res/10.png"));
    textureMap.assign(1, LoadTexture("res/9.png"));
    Texture2D textureCrit = LoadTexture("res/10.png");
    Texture2D textureDest = LoadTexture("res/9.png");

    //SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    srand(time(NULL));

    SpatialHashGrid grid({ 0, 0 }, { (float)screenWidth, (float)screenHeight }, 16, 8);

    // create some critters 
    Array<Critter> critters(200);
    ObjectPool critterPool(critters.getCount());

    const int MAX_VELOCITY = 80;

    for (int i = 0; i < critters.getCount(); i++) {
        // create a random position vector for the location
        Vector2 position = { (float)(5 + rand() % (screenWidth - 10)), (float)(5 + (rand() % screenHeight - 10)) };
        //position = { 100, 100 };
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

    //double startTime;
    //double timePassed;
    //double sumTime = 0;
    //double frames = 0;
    //double averageTime;
    
    // Main game loop
    while (!WindowShouldClose()) {   // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        float delta = GetFrameTime();
        //frames++

        //update the destroyer
        destroyer.Update(delta);
        // check destroyer against screen bounds
        if (destroyer.GetX() - destroyer.GetRadius() < 0) {
            destroyer.SetX(destroyer.GetRadius());
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetX() + destroyer.GetRadius() > screenWidth) {
            destroyer.SetX(screenWidth - destroyer.GetRadius());
            destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() - destroyer.GetRadius() < 0) {
            destroyer.SetY(destroyer.GetRadius());
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }
        if (destroyer.GetY() + destroyer.GetRadius() > screenHeight) {
            destroyer.SetY(screenHeight - destroyer.GetRadius());
            destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        }

        // update the critters
        // (dirty flags will be cleared during update)
        for (int i = 0; i < critterPool.getActive(); i++) {
            //int n = critterPool[i];
            Critter& critter = critters[critterPool[i]];
            critter.Update(delta);

            // check each critter against screen bounds
            if (critter.GetX() - critter.GetRadius() < 0) {
                critter.SetX(critter.GetRadius());
                critter.SetVelocity(Vector2{ -critter.GetVelocity().x, critter.GetVelocity().y });
            }
            if (critter.GetX() + critter.GetRadius() > screenWidth) {
                critter.SetX(screenWidth - critter.GetRadius());
                critter.SetVelocity(Vector2{ -critter.GetVelocity().x, critter.GetVelocity().y });
            }
            if (critter.GetY() - critter.GetRadius() < 0) {
                critter.SetY(critter.GetRadius());
                critter.SetVelocity(Vector2{ critter.GetVelocity().x, -critter.GetVelocity().y });
            }
            if (critter.GetY() + critter.GetRadius() > screenHeight) {
                critter.SetY(screenHeight - critter.GetRadius());
                critter.SetVelocity(Vector2{ critter.GetVelocity().x, -critter.GetVelocity().y });
            }

            // kill any critter touching the destroyer
            // simple circle-to-circle collision check
            float dist = Vector2Distance(critter.GetPosition(), destroyer.GetPosition());
            if (dist < critter.GetRadius() + destroyer.GetRadius()) {
                critterPool.unloadObject(i);
            }
        }

        // check for critter-on-critter collisions

        // spatial hashing setup
        Array<int> activeIDs(critterPool.getActive());
        for (int i = 0; i < critterPool.getActive(); i++) {
            activeIDs[i] = critterPool[i];
        }
        grid.generateHashList(critters, activeIDs);
        grid.sortByCellHash();
        grid.generateLookup();


        // spatial hashing collision logic
        const Array<int2>& hashList = grid.getHashList();
        Array<int> withinIDs(0, activeIDs.getCapacity());
        Array<int> nearbyIDs(0, activeIDs.getCapacity());

        for (int i = 0; i < hashList.getCount(); i++) {
            int currentCellHash = hashList[i].y;
            if (grid.isCellDirty(currentCellHash)) {
                i = grid.getEndIndices()[currentCellHash];
                continue;
            }

            withinIDs.quickCopy(grid.findWithin(currentCellHash));
            nearbyIDs.quickCopy(grid.findNearby(currentCellHash));

            grid.setCellDirty(currentCellHash);
            i = grid.getEndIndices()[currentCellHash];

            for (int j = 0; j < withinIDs.getCount(); j++) {
                Critter& critterA = critters[withinIDs[j]];
                if (critterA.IsDirty()) {
                    continue;
                }

                for (int n = 0; n < nearbyIDs.getCount(); n++) {
                    if (withinIDs[j] == nearbyIDs[n]) {
                        continue;
                    }

                    Critter& critterB = critters[nearbyIDs[n]];
                    float dist = Vector2Distance(critterA.GetPosition(), critterB.GetPosition());
                    if (dist < critterA.GetRadius() + critterB.GetRadius()) {
                        // collision!
                        // do math to get critters bouncing
                        Vector2 normal = Vector2Normalize(Vector2Subtract(critterB.GetPosition(), critterA.GetPosition()));

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
        }

        // former spatial hashing collision logic
        //for (int i = 0; i < critterPool.getActive(); i++) {
        //    Critter& critterA = critters[critterPool[i]];
        //    if (critterA.IsDirty()) {
        //        continue;
        //    }

        //    Array<int> nearbyIDs = grid.findNearby(grid.getCellPos(critterA.GetPosition()));
        //    for (int j = 0; j < nearbyIDs.getCount(); j++) {
        //        if (critterPool[i] == nearbyIDs[j]) {
        //            continue;
        //        }

        //        Critter& critterB = critters[nearbyIDs[j]];
        //        float dist = Vector2Distance(critterA.GetPosition(), critterB.GetPosition());
        //        if (dist < critterA.GetRadius() + critterB.GetRadius()) {
        //            // collision!
        //            // do math to get critters bouncing
        //            Vector2 normal = Vector2Normalize(Vector2Subtract(critterB.GetPosition(), critterA.GetPosition()));

        //            // not even close to real physics, but fine for our needs
        //            critterA.SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
        //            // set the critter to *dirty* so we know not to process any more collisions on it
        //            critterA.SetDirty();

        //            // we still want to check for collisions in the case where 1 critter is dirty - so we need a check 
        //            // to make sure the other critter is clean before we do the collision response
        //            if (!critterB.IsDirty()) {
        //                critterB.SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
        //                critterB.SetDirty();
        //            }
        //            break;
        //        }
        //    }
        //}

        //original collision logic
        //for (int i = 0; i < critterPool.getActive(); i++) {
        //    for (int j = 0; j < critterPool.getActive(); j++){
        //        Critter& critterA = critters[critterPool[i]];
        //        Critter& critterB = critters[critterPool[j]];
        //        if (i == j || critterA.IsDirty()) {
        //            continue;// note: the other critter (j) could be dirty - that's OK
        //        }

        //        // check every critter against every other critter
        //        float dist = Vector2Distance(critterA.GetPosition(), critterB.GetPosition());
        //        if (dist < critterA.GetRadius() + critterB.GetRadius()) {
        //            // collision!
        //            // do math to get critters bouncing
        //            Vector2 normal = Vector2Normalize( Vector2Subtract(critterB.GetPosition(), critterA.GetPosition()));

        //            // not even close to real physics, but fine for our needs
        //            critterA.SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
        //            // set the critter to *dirty* so we know not to process any more collisions on it
        //            critterA.SetDirty();

        //            // we still want to check for collisions in the case where 1 critter is dirty - so we need a check 
        //            // to make sure the other critter is clean before we do the collision response
        //            if (!critterB.IsDirty()) {
        //                critterB.SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
        //                critterB.SetDirty();
        //            }
        //            break;
        //        }
        //    }
        //}

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

        //grid.draw(critters);
        
        // draw the destroyer
        destroyer.Draw();

        // draw the critters
        for (int i = 0; i < critterPool.getActive(); i++) {
            critters[critterPool[i]].Draw();
        }

        DrawFPS(10, 10);
        //std::string debug = "Critters total: " + std::to_string(critterPool.getTotal()) + "\nCritters alive: " + std::to_string(critterPool.getActive()) + "\nCritters dead: " + std::to_string(critterPool.getInactive());
        //DrawText(debug.c_str(), 10, 32, 16, BLUE);

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