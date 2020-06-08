#pragma once

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

struct Coord
{
    long double lat, lon;
    int id = -1; //default value: -1
    bool isEqual(const Coord &d) const
    {
        return (id == d.id) && (lat == d.lat) && (lon == d.lon);
    }
};

struct less_than_key
{
    bool sortByLat = true;
    inline bool operator()(const Coord &d1, const Coord &d2)
    {
        if (sortByLat)
            return (d1.lat < d2.lat);
        else
            return (d1.lon < d2.lon);
    }
};

class ModelData
{
public:
    std::vector<Coord> list;
    bool sortByLat = true;
    int size()
    {
        return list.size();
    }
    void print()
    {
        for (int i = 0; i < list.size(); i++)
            std::cout << list[i].id << " " << list[i].lat << " " << list[i].lon << std::endl;
    }
};

namespace utils
{
    void reset_id(ModelData &data)
    {
        for (int i = 0; i < data.list.size(); i++)
            data.list[i].id = i;
    }

    ModelData read_data(std ::string const filename, bool sortLat)
    {
        std::fstream in(filename);
        std::string line;
        std::vector<Coord> data;
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
                Coord item = {lat, lon};
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
        reset_id(dataset);

        return dataset;
    }

}; // namespace utils

#endif