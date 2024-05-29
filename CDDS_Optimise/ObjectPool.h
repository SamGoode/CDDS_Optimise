#pragma once
#include "Array.h"

template <typename T>
class ObjectPool {
private:
    int totalCount;
    int activeCount;
    Array<T> objects;

public:
    ObjectPool() {}

    ObjectPool(int initCount) {
        totalCount = initCount;
        activeCount = initCount;
        objects = Array<T>(initCount);
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

    T& operator[](int index) {
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

    T& loadObject() {
        activeCount++;
        if (activeCount > totalCount) {
            totalCount++;
            objects.append(T());
        }

        return objects[activeCount - 1];
    }

    void unloadObject(int index) {
        if (index < 0 || index > activeCount - 1) {
            throw "object is not active";
        }

        objects[index] = objects[activeCount - 1];
        activeCount--;
    }
};