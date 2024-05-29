#pragma once
#include "Array.h"

// modified ObjectPool so it no longer stores the actual Objects
class ObjectPool {
private:
    int totalCount;
    int activeCount;
    // the array stores the index of each object within the original array
    Array<int> objects;

public:
    ObjectPool() {}

    ObjectPool(int initCount) {
        totalCount = initCount;
        activeCount = initCount;
        objects = Array<int>(initCount);

        for (int i = 0; i < objects.getCount(); i++) {
            objects[i] = i;
        }
    }

    ObjectPool(const ObjectPool& copy) {
        totalCount = copy.totalCount;
        activeCount = copy.activeCount;
        objects = copy.objects;
    }

    ObjectPool& operator=(const ObjectPool& copy) {
        totalCount = copy.totalCount;
        activeCount = copy.activeCount;
        objects = copy.objects;

        return *this;
    }

    int operator[](int index) {
        return objects[index];
    }

    int getTotal() {
        return totalCount;
    }

    int getActive() {
        return activeCount;
    }

    int getInactive() {
        return totalCount - activeCount;
    }

    int loadObject() {
        if (activeCount + 1 > totalCount) {
            throw "no more objects to load";
        }

        activeCount++;

        return objects[activeCount - 1];
    }

    void unloadObject(int index) {
        if (index < 0 || index > activeCount - 1) {
            throw "object is not active";
        }

        int temp = objects[index];
        objects[index] = objects[activeCount - 1];
        objects[activeCount - 1] = temp;
        activeCount--;
    }
};