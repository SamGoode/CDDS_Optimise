#pragma once
#include "raylib.h"

class TextureMap {
private:
    int count;
    Texture2D* ptr;

public:
    TextureMap();
    TextureMap(int _count);
    ~TextureMap();

    const Texture2D& operator[](int ID);

    void assign(int ID, Texture2D texture);

    int getCount();
};