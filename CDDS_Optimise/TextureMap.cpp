#include "TextureMap.h"

TextureMap::TextureMap() {
    count = 0;
    ptr = nullptr;
}

TextureMap::TextureMap(int _count) {
    count = _count;
    ptr = new Texture2D[count];
}

TextureMap::~TextureMap() {
    delete[] ptr;
}

const Texture2D& TextureMap::operator[](int ID) {
    if (ID < 0 || ID > count - 1) {
        throw "TextureMap out of bounds";
    }

    return ptr[ID];
}

void TextureMap::assign(int ID, Texture2D texture) {
    ptr[ID] = texture;
}

int TextureMap::getCount() {
    return count;
}