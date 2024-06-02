#pragma once
#include "Array.h"

// modified ObjectPool so it no longer stores the actual Objects
template <typename T>
class ObjectPool : public Array<T> {
private:
    int activeCount;

public:
    ObjectPool() {}

    ObjectPool(unsigned int initCount) : Array<T>(initCount) {
        activeCount = initCount;
    }

    ObjectPool(const ObjectPool& copy) : Array<T>(copy) {
        activeCount = copy.activeCount;
    }

    ObjectPool& operator=(const ObjectPool& copy) {
        Array<T>::operator=(copy);
        activeCount = copy.activeCount;

        return *this;
    }

    int getTotal() {
        return Array<T>::getCount();
    }

    int getActive() {
        return activeCount;
    }

    int getInactive() {
        return Array<T>::getCount() - activeCount;
    }

    int loadObject() {
        if (activeCount + 1 > Array<T>::getCount()) {
            throw "no more objects to load";
        }

        activeCount++;
        return Array<T>::operator[](activeCount - 1);
    }

    void unloadObject(int index) {
        if (index < 0 || index > activeCount - 1) {
            throw "object is not active";
        }

        activeCount--;
        int temp = Array<T>::operator[](index);
        Array<T>::operator[](index) = Array<T>::operator[](activeCount);
        Array<T>::operator[](activeCount) = temp;
    }
};