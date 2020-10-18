//
// test_rtree.cpp
//
// This is a direct port of the C version of the RTree test program.
//

#include "rtree.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>

struct Rect
{
    Rect() {}

    Rect(long double a_minX, long double a_minY, long double a_maxX,
         long double a_maxY)
    {
        min[0] = a_minX;
        min[1] = a_minY;

        max[0] = a_maxX;
        max[1] = a_maxY;
    }

    long double min[2];
    long double max[2];
};

bool MySearchCallback(int id, void *arg)
{
    // printf("Hit data rect %d\n", id);
    return true; // keep going
}

void is_valid_file(const std::string filename)
{
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0)
    {
        std::cout << "Error reading " << filename << std::endl;
        exit(1);
    };
}

void remove_repeated(std::vector<Rect> *data)
{
    data->erase(std::unique(data->begin(), data->end(),
                            [](const Rect &lhs, const Rect &rhs) {
                                return lhs.min[0] == rhs.min[0] &&
                                       lhs.min[1] == rhs.min[1] &&
                                       lhs.max[0] == rhs.max[0] &&
                                       lhs.max[1] == rhs.max[1];
                            }),
                data->end());
}

template <typename T>
bool read_data(std ::string const &filename, std::vector<Rect> *data)
{
    is_valid_file(filename);
    std::cout << "Reading from " << filename << std::endl;
    std::fstream in(filename);
    std::string line;
    T lat;
    T lon;

    while (std::getline(in, line))
    {
        std::stringstream ss(line);
        if (!(ss >> lat >> lon))
            break;
        data->push_back(Rect(lat, lon, lat, lon));
    }

    remove_repeated(data);

    return true;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " input file missing" << std::endl;
        return 1;
    }

    std::vector<Rect> rects;
    bool is_read = read_data<long double>(argv[1], &rects);

    int nrects = rects.size();

    RTree<int, long double, 2, float> tree;

    int i, nhits;
    printf("number of rects = %d\n", nrects);

    auto start = std::chrono::high_resolution_clock::now();

    for (i = 0; i < nrects; i++)
    {
        tree.Insert(
            rects[i].min, rects[i].max,
            i); // Note, all values including zero are fine in this version
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Build time: " << duration.count() << " nanoseconds\n"
              << std::endl;

    // Range Serch

    long double start_n = 0.5;
    long double end_n = 0.5;
    for (int i = 0; i < 10; i++)
    {
        long double step = i * 0.05;
        long double range_min = start_n - step;
        long double range_max = end_n + step;
        std::cout << "Search Range: " << range_min << " - " << range_max
                  << std::endl;
        Rect search_rect(range_min, range_min, range_max, range_max);

        start = std::chrono::high_resolution_clock::now();
        nhits = tree.Search(search_rect.min, search_rect.max, MySearchCallback,
                            NULL);
        end = std::chrono::high_resolution_clock::now();
        duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        std::cout << "time: " << duration.count() << " nanoseconds\n"
                  << "Search resulted in " << nhits << " hits\n"
                  << std::endl;
    }

    // Iterator test
    // int itIndex = 0;
    // RTree<int, long double, 2, float>::Iterator it;
    // int count = 0;
    // for (tree.GetFirst(it); !tree.IsNull(it); tree.GetNext(it))
    // {
    //     int value = tree.GetAt(it);

    //     long double boundsMin[2] = {0.5, 0.5};
    //     long double boundsMax[2] = {0.55, 0.55};
    //     it.GetBounds(boundsMin, boundsMax);
    //     printf("it[%d] %d = (%Lf,%Lf,%Lf,%Lf)\n", itIndex++, value,
    //            boundsMin[0], boundsMin[1], boundsMax[0], boundsMax[1]);
    //     count++;
    // }
    // std::cout << "Counts: " << count << std::endl;

    std::vector<Rect> search_rects = rects;

    start = std::chrono::high_resolution_clock::now();
    for (i = 0; i < search_rects.size(); i++)
        nhits = tree.Search(search_rects[i].min, search_rects[i].max,
                            MySearchCallback, NULL);
    // printf("Search resulted in %d hits\n", nhits);

    end = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Searching time: " << duration.count() << " nanoseconds\n"
              << "Average look up time: "
              << duration.count() / search_rects.size() << " nanoseconds"
              << std::endl;

    return 0;
}
