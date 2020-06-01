#ifndef UTILS_H
#define UTILS_H

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

struct DataItem
{
    long double lat, lon;
    // bool sortLat = true;
    // bool operator<(const DataItem &item, bool sortLat) const
    // {
    //     if (sortLat)
    //         return (lat < item.lat);
    //     else
    //         return (lon < item.lon);
    // }
};

struct less_than_key
{
    bool sortByLat = true;
    inline bool operator()(const DataItem &d1, const DataItem &d2)
    {
        if (sortByLat)
            return (d1.lat < d2.lat);
        else
            return (d1.lon < d2.lon);
    }
};

struct ModelData
{
    std::vector<DataItem> list;
    bool sortByLat = true;
};

namespace utils
{
    ModelData read_data(std ::string const filename, bool sortLat)
    {
        std::fstream in(filename);
        std::string line;
        std::vector<DataItem> data;
        std::set<long double> latCounter;
        std::set<long double> lonCounter;

        int total = 0;
        while (std::getline(in, line))
        {
            long double lat;
            long double lon;
            std::stringstream ss(line);

            while (ss >> lat >> lon)
            {
                DataItem item = {lat, lon};
                data.push_back(item);
                latCounter.insert(lat);
                lonCounter.insert(lat);
            }
            ++total;
        }

        // Check if # unique value of latitude is larger than # unique value of
        // lontitude
        ModelData dataset;
        bool sortByLat = true;
        if (latCounter.size() < lonCounter.size())
            sortByLat = false;

        // Sort data
        std::sort(data.begin(), data.end(), less_than_key{sortByLat});
        dataset = {data, sortByLat};

        return dataset;
    }
}; // namespace utils

#endif