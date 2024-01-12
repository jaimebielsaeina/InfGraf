#ifndef SPACESECTIONER_H
#define SPACESECTIONER_H
#pragma once

#include <mutex>
using namespace std;

// Object that divides a given space into sections.
class SpaceSectioner {

    // Space to section.
    int width, height, wSections, hSections;
    int i, j, iter, total;

    // False while there are sections to get.
    bool done = false;

    // Concurrent access to the sectioner.
    mutex mMutex;

public:

    // Constructor.
    SpaceSectioner(int width, int height, int wSections, int hSections) :
        width(width), height(height), wSections(wSections), hSections(hSections),
        i(0), j(0), iter(0), total(wSections*hSections), done (false) {}

    // Returns false if there are no more sections to get.
    // Otherwise, returns true and sets the section limits.
    bool getSection(int& minX, int& maxX, int& minY, int& maxY) {
        mMutex.lock();
        if (done) {
            mMutex.unlock();
            return false;
        }
        minX = i * width / wSections;
        maxX = (i + 1) * width / wSections;
        minY = j * height / hSections;
        maxY = (j + 1) * height / hSections;
        cout << ++iter << " / " << total << endl;
        if (++j == hSections) {
            j = 0;
            if (++i == wSections) done = true;
        }
        mMutex.unlock();
        return true;
    }

};

#endif // SPACESECTIONER_H