#pragma once
#include <string>
#include <algorithm>
#include "raymath.h"
#include "Array.h"
#include "Critter.h"
#include "int2.h"
#include "RadixSort.h"

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
    int cellCount;
    // (particle ID, cell ID/hash)
    Array<int2> hashList;
    Array<int> startIndices;
    Array<int> endIndices;
    Array<bool> dirty;

public:
    SpatialHashGrid() {}

    SpatialHashGrid(Vector2 _pos, Vector2 _size, int _gridWidth, int _gridHeight) {
        m_pos = _pos;
        m_size = _size;
        gridWidth = _gridWidth;
        gridHeight = _gridHeight;
        cellWidth = (float)(m_size.x / gridWidth);
        cellHeight = (float)(m_size.y / gridHeight);
        cellCount = gridWidth * gridHeight;
    }

    void insertPositions(const Array<Critter>& critters, Array<int> IDs) {
        hashList = Array<int2>(IDs.getCount());

        for (int i = 0; i < hashList.getCount(); i++) {
            hashList[i] = { IDs[i], getCellHash(getCellPos(critters[IDs[i]].GetPosition())) };
        }
    }

    void sortByCellHash() {
        if (hashList.getCount() == 0) {
            return;
        }

        //std::sort(&hashList[0], &hashList[hashList.getCount() - 1], compareCellHash);
        hashList = radixSort(hashList, 127);
    }

    void generateLookup() {
        startIndices = Array<int>(gridWidth * gridHeight);
        endIndices = Array<int>(gridWidth * gridHeight);
        dirty = Array<bool>(gridWidth * gridHeight, false);

        int currentStart = 0;
        int previousCellHash = -1;
        for (int i = 0; i < hashList.getCount(); i++) {
            if (i == hashList.getCount() - 1) {
                startIndices[hashList[i].y] = currentStart;
                endIndices[hashList[i].y] = currentStart;
                break;
            }

            if (hashList[i].y == hashList[i + 1].y) {
                continue;
            }

            startIndices[hashList[i].y] = currentStart;

            if (previousCellHash != -1) {
                endIndices[previousCellHash] = currentStart - 1;
            }
            previousCellHash = hashList[i].y;

            currentStart = i + 1;
        }
    }

    const Array<int2>& getHashList() {
        return hashList;
    }

    Array<int> findWithin(int cellHash) {
        Array<int> IDs(0, hashList.getCapacity(), -1);
        int startIndex = startIndices[cellHash];

        for (int i = startIndex; i < hashList.getCount() && hashList[i].y == cellHash; i++) {
            IDs.append(hashList[i].x);
        }

        return IDs;
    }

    void setCellDirty(int cellHash) {
        dirty[cellHash] = true;
    }

    bool isCellDirty(int cellHash) {
        return dirty[cellHash];
    }

    const Array<int>& getStartIndices() {
        return startIndices;
    }

    const Array<int>& getEndIndices() {
        return endIndices;
    }

    Array<int> getNearbyHashes(int cellHash) {
        Array<int> hashes(0, 16, -1);

        int offsets[9] = { 
            -1 - gridWidth, -gridWidth, 1 - gridWidth,
            -1, 0, 1,
            -1 + gridWidth, gridWidth, 1 + gridWidth
        };

        for (int i = 0; i < hashes.getCount(); i++) {
            if (cellHash + offsets[i] < 0 || cellHash + offsets[i] >= cellCount) {
                continue;
            }
            if (isCellDirty(cellHash + offsets[i])) {
                continue;
            }
            hashes.append(cellHash + offsets[i]);
        }

        return hashes;
    }

    Array<int> findNearby(int cellHash) {
        Array<int> IDs(0, hashList.getCapacity(), -1);
        Array<int> nearbyHashes = getNearbyHashes(cellHash);

        for (int i = 0; i < nearbyHashes.getCount(); i++) {
           IDs.append(findWithin(nearbyHashes[i]));
        }

        return IDs;
    }

    Array<int> findNearby(int2 cellPos) {
        Array<int> IDs(0, hashList.getCapacity(), -1);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int2 queryPos = { cellPos.x + i - 1, cellPos.y + j - 1 };
                if (queryPos.x < 0 || queryPos.x >= gridWidth || queryPos.y < 0 || queryPos.y >= gridHeight) {
                    continue;
                }

                int cellHash = getCellHash(queryPos);
                int startIndex = startIndices[cellHash];

                for (int n = startIndex; n < hashList.getCount() && hashList[n].y == cellHash; n++) {
                    IDs.append(hashList[n].x);
                }
            }
        }

        return IDs;
    }

    bool isValidPos(Vector2 pos) {
        return (pos.x >= m_pos.x && pos.x < m_pos.x + m_size.x && pos.y >= m_pos.y && pos.y < m_pos.y + m_size.y);
    }

    int2 getCellPos(Vector2 pos) {
        Vector2 adjusted = Vector2Subtract(pos, m_pos);
        int2 cellPos = { (int)floor(adjusted.x / cellWidth), (int)floor(adjusted.y / cellHeight) };
        if (cellPos.x < 0) {
            cellPos.x = 0;
        }
        else if (cellPos.x >= gridWidth) {
            cellPos.x = gridWidth - 1;
        }

        if (cellPos.y < 0) {
            cellPos.y = 0;
        }
        else if (cellPos.y >= gridHeight) {
            cellPos.y = gridHeight - 1;
        }

        return cellPos;
    }

    int getCellHash(int2 cellPos) {
        return (cellPos.x + cellPos.y * gridWidth);
    }

    void draw(const Array<Critter>& critters) {
        //Vector2 pos = GetMousePosition();
        Vector2 pos = critters[0].GetPosition();

        int2 cellPos = getCellPos(pos);
        Array<int> IDs = findNearby(cellPos);

        int x = cellPos.x;
        int y = cellPos.y;
        DrawRectangle(m_pos.x + (x - 1) * cellWidth, m_pos.y + (y - 1) * cellHeight, cellWidth * 3, cellHeight * 3, ORANGE);
        DrawRectangle(m_pos.x + x * cellWidth, m_pos.y + y * cellHeight, cellWidth, cellHeight, RED);

        for (int i = 0; i < gridWidth; i++) {
            for (int n = 0; n < gridHeight; n++) {
                DrawRectangleLines(round(m_pos.x + i * cellWidth), round(m_pos.y + n * cellHeight), cellWidth, cellHeight, BLACK);
            }
        }

        DrawRectangleLines(m_pos.x, m_pos.y, m_size.x, m_size.y, BLUE);

        for (int i = 0; i < hashList.getCount(); i++) {
            bool isNearby = false;
            for (int n = 0; n < IDs.getCount(); n++) {
                if (hashList[i].x == IDs[n]) {
                    isNearby = true;
                }
            }

            Vector2 c_pos = critters[hashList[i].x].GetPosition();

            if (isNearby) {
                DrawCircle(c_pos.x, c_pos.y, 15, RED);
            }
            else {
                DrawCircle(c_pos.x, c_pos.y, 15, GREEN);
            }
        }

        DrawCircle(pos.x, pos.y, 15, BLUE);

        std::string cellID = "Cell ID: " + std::to_string(getCellHash(cellPos));
        DrawText(cellID.c_str(), 10, 100, 20, PURPLE);
        std::string nearby = "Within bounds: " + std::to_string(IDs.getCount());
        DrawText(nearby.c_str(), 10, 140, 20, PURPLE);
        std::string IDsText = "IDs: [";
        for (int i = 0; i < IDs.getCount(); i++) {
            IDsText += std::to_string(IDs[i]);
            if (i != IDs.getCount() - 1) {
                IDsText += ",";
            }
        }
        IDsText += "]";
        DrawText(IDsText.c_str(), 200, 20, 20, PURPLE);

        std::string valid = isValidPos(GetMousePosition()) ? "true" : "false";
        valid = "Valid location: " + valid;
        DrawText(valid.c_str(), 10, 160, 20, PURPLE);
    }
};