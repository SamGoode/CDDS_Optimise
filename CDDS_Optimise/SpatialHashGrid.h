#pragma once
#include <string>
#include <algorithm>
#include "raymath.h"
#include "Array.h"
#include "Critter.h"
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
    // (particle ID, cell ID)
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

        std::sort(&hashList[0], &hashList[hashList.getCount() - 1] + 1, compareCellHash);
    }

    void generateLookup() {
        lookup = Array<int>(gridWidth * gridHeight);
        for (int i = 0; i < lookup.getCount(); i++) {
            lookup[i] = -1;
        }

        int currentStart = 0;
        for (int i = 0; i < hashList.getCount(); i++) {
            if (i == hashList.getCount() - 1) {
                lookup[hashList[i].y] = currentStart;
                continue;
            }

            if (hashList[i].y == hashList[i + 1].y) {
                continue;
            }

            lookup[hashList[i].y] = currentStart;
            currentStart = i + 1;
        }
    }

    Array<int> findNearby(int2 cellPos) {
        Array<int> IDs;

        for (int i = 0; i < 3; i++) {
            for (int n = 0; n < 3; n++) {
                int2 queryPos = { cellPos.x + i - 1, cellPos.y + n - 1 };
                if (queryPos.x < 0 || queryPos.x >= gridWidth || queryPos.y < 0 || queryPos.y >= gridHeight) {
                    continue;
                }

                int cellHash = getCellHash(queryPos);
                int startIndex = lookup[cellHash];

                for (int i = startIndex; i < hashList.getCount() && hashList[i].y == cellHash; i++) {
                    IDs.append(hashList[i].x);
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