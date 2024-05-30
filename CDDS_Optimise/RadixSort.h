#pragma once
#include "Array.h"
#include "int2.h"

// radix sort is O(d(n + w)) where w is size of digit and d is number of digits
// for 128 different cell IDs I want to find the sweet spot
// w = 16, (base 16) 4 bits
// 127 = 0x7F
// 2 digits
// w = 10 (decimal)
// 3 digits
// w = 2, (binary) 1 bit
// 127 = 01111111
// 7 digits

// radix sort
Array<int> radixSort(Array<int> unsorted) {
    int maxValue = 0;
    for (int i = 0; i < unsorted.getCount(); i++) {
        if (maxValue < unsorted[i]) {
            maxValue = unsorted[i];
        }
    }

    for (int i = 0; (1 << (i * 4)) < maxValue; i++) {
        // contains the range of possible values found in a digit
        Array<int> count(16, 0);

        for (int n = 0; n < unsorted.getCount(); n++) {
            char halfByte = (unsorted[n] >> (i * 4)) & 0x0000000F;
            count[halfByte] += 1;
        }

        for (int n = 1; n < count.getCount(); n++) {
            count[n] += count[n - 1];
        }

        Array<int> sorted(unsorted.getCount());
        for (int n = unsorted.getCount() - 1; n >= 0; n--) {
            char halfByte = (unsorted[n] >> (i * 4)) & 0x0000000F;
            sorted[count[halfByte] - 1] = unsorted[n];
            count[halfByte] -= 1;
        }

        unsorted = sorted;
    }

    return unsorted;
}

Array<int2> radixSort(Array<int2> unsorted) {
    int maxValue = 0;
    for (int i = 0; i < unsorted.getCount(); i++) {
        if (maxValue < unsorted[i].y) {
            maxValue = unsorted[i].y;
        }
    }

    for (int i = 0; (1 << (i * 4)) < maxValue; i++) {
        // contains the range of possible values found in a digit
        Array<int> count(16, 0);

        for (int n = 0; n < unsorted.getCount(); n++) {
            char halfByte = (unsorted[n].y >> (i * 4)) & 0x0000000F;
            count[halfByte] += 1;
        }

        for (int n = 1; n < count.getCount(); n++) {
            count[n] += count[n - 1];
        }

        Array<int2> sorted(unsorted.getCount());
        for (int n = unsorted.getCount() - 1; n >= 0; n--) {
            char halfByte = (unsorted[n].y >> (i * 4)) & 0x0000000F;
            sorted[count[halfByte] - 1] = unsorted[n];
            count[halfByte] -= 1;
        }

        unsorted = sorted;
    }

    return unsorted;
}

Array<int2> radixSort(Array<int2> unsorted, int maxValue) {
    for (int i = 0; (1 << (i * 4)) < maxValue; i++) {
        // contains the range of possible values found in a digit
        Array<int> count(16, 0);

        for (int n = 0; n < unsorted.getCount(); n++) {
            char halfByte = (unsorted[n].y >> (i * 4)) & 0x0000000F;
            count[halfByte] += 1;
        }

        for (int n = 1; n < count.getCount(); n++) {
            count[n] += count[n - 1];
        }

        Array<int2> sorted(unsorted.getCount());
        for (int n = unsorted.getCount() - 1; n >= 0; n--) {
            char halfByte = (unsorted[n].y >> (i * 4)) & 0x0000000F;
            sorted[count[halfByte] - 1] = unsorted[n];
            count[halfByte] -= 1;
        }

        unsorted = sorted;
    }

    return unsorted;
}


// count sort
//Array<int> unsorted(8);
//int values[8] = { 5, 13, 2, 11, 5, 4, 11, 3 };
//for (int i = 0; i < unsorted.getCount(); i++) {
//    unsorted[i] = values[i];
//}
//DrawText(unsorted.toString().c_str(), 200, 10, 16, BLUE);

//// contains the range of possible values found in unsorted array
//Array<int> count(16, 0);
//for (int i = 0; i < unsorted.getCount(); i++) {
//    count[unsorted[i]] += 1;
//}
//DrawText(count.toString().c_str(), 200, 30, 16, BLUE);

//for (int i = 1; i < count.getCount(); i++) {
//    count[i] += count[i - 1];
//}
//DrawText(count.toString().c_str(), 200, 50, 16, BLUE);

//Array<int> sorted(unsorted.getCount());
//for (int i = unsorted.getCount() - 1; i >= 0; i--) {
//    sorted[count[unsorted[i]] - 1] = unsorted[i];
//    count[unsorted[i]] -= 1;
//}
//DrawText(sorted.toString().c_str(), 200, 70, 16, BLUE);