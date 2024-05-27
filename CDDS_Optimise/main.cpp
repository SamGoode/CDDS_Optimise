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

    // create some critters 
    //Critter critters[50];
    //CritterPool critters(50);
    //ObjectPool<Critter> critters(50);
    
    int destroyerCount = 2;
    int critterCount = 48;
    ObjectPool<CollisionObject*> collisionPool(destroyerCount + critterCount);

    const int MAX_VELOCITY = 80;

    for (int i = 0; i < collisionPool.getTotal(); i++) {
        if (i < destroyerCount) {
            Vector2 velocity = { -100 + (rand() % 200), -100 + (rand() % 200) };
            velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

            collisionPool[i] = new Destroyer();
            collisionPool[i]->Init(Vector2{(float)(screenWidth >> 1), (float)(screenHeight >> 1)}, velocity, 28, &textureMap, 1);
            continue;
        }

        // create a random position vector for the location
        Vector2 position = { (float)(5 + rand() % (screenWidth - 10)), (float)(5 + (rand() % screenHeight - 10)) };

        // create a random direction vector for the velocity
        Vector2 velocity = { -100+(rand()%200), -100+(rand()%200) };
        // normalize and scale by a random speed
        velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);

        // create a critter in a random location
        collisionPool[i] = new Critter();
        collisionPool[i]->Init(position, velocity, 14, &textureMap, 0);
    }

    //Destroyer destroyer;
    //Vector2 velocity = { -100 + (rand() % 200), -100 + (rand() % 200) };
    //velocity = Vector2Scale(Vector2Normalize(velocity), MAX_VELOCITY);
    //destroyer.Init(Vector2{ (float)(screenWidth >> 1), (float)(screenHeight >> 1) }, velocity, 28, &textureMap, 1);

    float timer = 1;
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        float delta = GetFrameTime();

        // update the destroyer
        //destroyer.Update(delta);
        //// check destroyer against screen bounds
        //if (destroyer.GetX() < 0) {
        //    destroyer.SetX(0);
        //    destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        //}
        //if (destroyer.GetX() > screenWidth) {
        //    destroyer.SetX(screenWidth);
        //    destroyer.SetVelocity(Vector2{ -destroyer.GetVelocity().x, destroyer.GetVelocity().y });
        //}
        //if (destroyer.GetY() < 0) {
        //    destroyer.SetY(0);
        //    destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        //}
        //if (destroyer.GetY() > screenHeight) {
        //    destroyer.SetY(screenHeight);
        //    destroyer.SetVelocity(Vector2{ destroyer.GetVelocity().x, -destroyer.GetVelocity().y });
        //}

        // update the critters
        // (dirty flags will be cleared during update)
        for (int i = 0; i < collisionPool.getActive(); i++) {
            collisionPool[i]->Update(delta);

            // check each critter against screen bounds
            if (collisionPool[i]->GetX() < 0) {
                collisionPool[i]->SetX(0);
                collisionPool[i]->SetVelocity(Vector2{ -collisionPool[i]->GetVelocity().x, collisionPool[i]->GetVelocity().y });
            }
            if (collisionPool[i]->GetX() > screenWidth) {
                collisionPool[i]->SetX(screenWidth);
                collisionPool[i]->SetVelocity(Vector2{ -collisionPool[i]->GetVelocity().x, collisionPool[i]->GetVelocity().y });
            }
            if (collisionPool[i]->GetY() < 0) {
                collisionPool[i]->SetY(0);
                collisionPool[i]->SetVelocity(Vector2{ collisionPool[i]->GetVelocity().x, -collisionPool[i]->GetVelocity().y });
            }
            if (collisionPool[i]->GetY() > screenHeight) {
                collisionPool[i]->SetY(screenHeight);
                collisionPool[i]->SetVelocity(Vector2{ collisionPool[i]->GetVelocity().x, -collisionPool[i]->GetVelocity().y });
            }

            // kill any critter touching the destroyer
            // simple circle-to-circle collision check
            //float dist = Vector2Distance(collisionPool[i]->GetPosition(), collisionPool[0]->GetPosition());
            //if (dist < collisionPool[i]->GetRadius() + collisionPool[0]->GetRadius()) {
            //    collisionPool.unloadObject(i);
            //}
        }

        // kill any critter touching any destroyers
        // simple circle-to-circle collision check
        for (int i = 0; i < destroyerCount; i++) {
            for (int j = destroyerCount; j < collisionPool.getActive(); j++) {
                float dist = Vector2Distance(collisionPool[i]->GetPosition(), collisionPool[j]->GetPosition());
                if (dist < collisionPool[i]->GetRadius() + collisionPool[j]->GetRadius()) {
                    collisionPool.unloadObject(j);
                }
            }
        }
                
        // check for critter-on-critter collisions
        for (int i = destroyerCount; i < collisionPool.getActive(); i++) {
            for (int j = destroyerCount; j < collisionPool.getActive(); j++){
                if (i == j || collisionPool[i]->IsDirty()) {
                    continue;// note: the other critter (j) could be dirty - that's OK
                }

                // check every critter against every other critter
                float dist = Vector2Distance(collisionPool[i]->GetPosition(), collisionPool[j]->GetPosition());
                if (dist < collisionPool[i]->GetRadius() + collisionPool[j]->GetRadius()) {
                    // collision!
                    // do math to get critters bouncing
                    Vector2 normal = Vector2Normalize( Vector2Subtract(collisionPool[j]->GetPosition(), collisionPool[i]->GetPosition()));

                    // not even close to real physics, but fine for our needs
                    collisionPool[i]->SetVelocity(Vector2Scale(normal, -MAX_VELOCITY));
                    // set the critter to *dirty* so we know not to process any more collisions on it
                    collisionPool[i]->SetDirty();

                    // we still want to check for collisions in the case where 1 critter is dirty - so we need a check 
                    // to make sure the other critter is clean before we do the collision response
                    if (!collisionPool[j]->IsDirty()) {
                        collisionPool[j]->SetVelocity(Vector2Scale(normal, MAX_VELOCITY));
                        collisionPool[j]->SetDirty();
                    }
                    break;
                }
            }
        }

        timer -= delta;
        if (timer <= 0) {
            timer = 1;

            // find any dead critters and spit them out (respawn)
            //for (int i = 0; i < CRITTER_COUNT; i++) {
            //    if (critters[i].IsDead()) {
            //        Vector2 normal = Vector2Normalize(destroyer.GetVelocity());

            //        // get a position behind the destroyer, and far enough away that the critter won't bump into it again
            //        Vector2 pos = destroyer.GetPosition();
            //        pos = Vector2Add(pos, Vector2Scale(normal, -50));

            //        // its pretty inefficient to keep reloading textures. ...if only there was something else we could do
            //        critters[i].Init(pos, Vector2Scale(normal, -MAX_VELOCITY), 12, textures[0]);
            //        break;
            //    }
            //}
            //nextSpawnPos = destroyer.GetPosition();

            if (collisionPool.getInactive() > 0) {
                // get a position behind the destroyer, and far enough away that the critter won't bump into it again
                Vector2 normal = Vector2Normalize(collisionPool[0]->GetVelocity());
                Vector2 pos = collisionPool[0]->GetPosition();
                pos = Vector2Add(pos, Vector2Scale(normal, -50));

                collisionPool.loadObject()->InitVecs(pos, Vector2Scale(normal, -MAX_VELOCITY));
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // draw the critters
        for (int i = 0; i < collisionPool.getActive(); i++) {
            collisionPool[i]->Draw();
        }
        // draw the destroyer
        //destroyer.Draw();

        DrawFPS(10, 10);
        std::string debug = "Critters total: " + std::to_string(collisionPool.getTotal()) + "\nCritters alive: " + std::to_string(collisionPool.getActive()) + "\nCritters dead: " + std::to_string(collisionPool.getInactive());
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