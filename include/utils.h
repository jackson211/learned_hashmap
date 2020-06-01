#ifndef UTILS_H
#define UTILS_H

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
    bool sortLat = true;
    bool operator<(const DataItem &item) const
    {
        if (sortLat)
        {
            return (lat < item.lat);
        }
        else
        {
            return (lon < item.lon);
        }
    }
};

struct Data{
    int id;

}

namespace utils
{
    void log(std::vector<DataItem> const &data)
    {
        for (int i = 0; i < data.size(); i++)
        {
            std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << data.at(i).lat << ", " << data.at(i).lon;
            std::cout << "\n";
        }
    }

    std::vector<DataItem> read_data(std ::string const filename, bool sortLat)
    {
        std::fstream in(filename);
        std::string line;
        std::vector<DataItem> data;

        int total = 0;
        while (std::getline(in, line))
        {
            long double lat;
            long double lon;
            std::stringstream ss(line);

            while (ss >> lat >> lon)
            {
                DataItem item = {lat, lon, sortLat};
                data.push_back(item);
            }
            ++total;
        }
        // Sort data
        std::sort(data.begin(), data.end());
        return data;
    }
}; // namespace utils

#endif