#include <mutex>
using namespace std;

class SpaceSectioner {

    // mutex for the space sectioner
    mutex mMutex;

    int width, height, wSections, hSections;
    int i, j, iter, total;
    bool done = false;

public:

    SpaceSectioner(int width, int height, int wSections, int hSections) :
        width(width), height(height), wSections(wSections), hSections(hSections),
        i(0), j(0), iter(0), total(wSections*hSections), done (false) {}

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