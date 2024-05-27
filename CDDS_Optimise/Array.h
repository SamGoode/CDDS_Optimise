#pragma once

template <typename T>
class Array {
private:
    unsigned int count;
    unsigned int capacity;
    T* ptr;

public:
    Array() {
        count = 0;
        capacity = 1;
        ptr = new T[capacity];
    }

    Array(unsigned int initCount) {
        count = initCount;

        capacity = 1;
        for (unsigned int i = 0; i < 32; i++) {
            if ((capacity << i) >= initCount) {
                capacity <<= i;
                break;
            }
        }
        ptr = new T[capacity];
    }

    Array(const Array& copy) {
        count = copy.count;
        capacity = copy.capacity;
        ptr = new T[capacity];
        for (unsigned int i = 0; i < count; i++) {
            ptr[i] = copy.ptr[i];
        }
    }

    ~Array() {
        delete[] ptr;
    }

    Array& operator=(const Array& copy) {
        delete[] ptr;

        count = copy.count;
        capacity = copy.capacity;
        ptr = new T[capacity];
        for (unsigned int i = 0; i < count; i++) {
            ptr[i] = copy.ptr[i];
        }

        return *this;
    }

    T& operator[](unsigned int index) {
        if (index < 0 || index > count - 1) {
            throw "array out of bounds";
        }

        return ptr[index];
    }

    unsigned int getCount() {
        return count;
    }

    unsigned int getCapacity() {
        return capacity;
    }

    void resize() {
        T* oldPtr = ptr;

        capacity <<= 1;
        ptr = new T[capacity];
        for (unsigned int i = 0; i < count; i++) {
            ptr[i] = oldPtr[i];
        }
        delete[] oldPtr;
    }

    void append(T newObj) {
        if (count + 1 > capacity) {
            resize();
        }
        count++;
        ptr[count - 1] = newObj;
    }

    void remove(unsigned int index) {
        if (index < 0 || index > count - 1) {
            throw "array out of bounds";
        }

        count--;
        for (unsigned int i = index; i < count; i++) {
            ptr[i] = ptr[i + 1];
        }
        ptr[count] = T();
    }
};