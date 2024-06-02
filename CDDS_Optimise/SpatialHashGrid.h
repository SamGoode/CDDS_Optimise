#pragma once
#include "raymath.h"
#include "Array.h"
#include "Critter.h"
#include "int2.h"

class SpatialHashGrid {
private:
    Vector2 m_pos;
    Vector2 m_size;
    int gridWidth;
    int gridHeight;
    float cellWidth;
    float cellHeight;
    int cellCount;
    int2 posOffsets[9];
    int hashOffsets[9];
    // (particle ID, cell ID/hash)
    Array<int2> hashList;
    Array<int> startIndices;
    Array<int> endIndices;
    Array<bool> dirty;
    Array<int> tempIDs;

public:
    SpatialHashGrid();

    SpatialHashGrid(Vector2 _pos, Vector2 _size, int _gridWidth, int _gridHeight);

    const Array<int2>& getHashList();
    const Array<int>& getStartIndices();
    const Array<int>& getEndIndices();

    void setCellDirty(int cellHash);
    bool isCellDirty(int cellHash);

    bool isValidCellPos(int2 cellPos);
    bool isValidPos(Vector2 pos);
    int2 getCellPos(int cellHash);
    int2 getCellPos(Vector2 pos);

    int getCellHash(int2 cellPos);

    void generateHashList(const Array<Critter>& critters, Array<int> IDs);
    void sortByCellHash();
    void generateLookup();

    const Array<int>& findWithin(int cellHash);
    const Array<int>& findNearby(int centreCellHash);
    const Array<int>& findNearby(int2 cellPos);

    void draw(const Array<Critter>& critters);
};