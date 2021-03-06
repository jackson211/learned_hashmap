#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "entry.h"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

namespace utils
{

    void is_valid_file(const std::string filename)
    {
        struct stat buffer;
        if (stat(filename.c_str(), &buffer) != 0)
        {
            std::cout << "Error reading " << filename << std::endl;
            exit(1);
        };
    }

    void reset_id(std::vector<Point> *data)
    {
        for (size_t i = 0; i < data->size(); i++)
        {
            id_type id = i;
            data->at(i).setId(id);
        }
    }

    void remove_repeated(std::vector<Point> *data)
    {
        data->erase(std::unique(data->begin(), data->end(),
                                [](const Point &lhs, const Point &rhs) {
                                    return lhs.lon == rhs.lon &&
                                           lhs.lat == rhs.lat;
                                }),
                    data->end());
    }

    void sort_data(bool sort_by_lat, std::vector<Point> *data)
    {
        sort_by_lat ? std::sort(data->begin(), data->end(),
                                [](const Point &lhs, const Point &rhs) {
                                    return lhs.lat < rhs.lat;
                                })
                    : std::sort(data->begin(), data->end(),
                                [](const Point &lhs, const Point &rhs) {
                                    return lhs.lon < rhs.lon;
                                });
    }

    long double getVariance(std::vector<long double> &data)
    {

        long double sum = std::accumulate(data.begin(), data.end(), 0.0);
        long double mean = sum / data.size();
        std::vector<long double> diff(data.size());
        std::transform(data.begin(), data.end(), diff.begin(),
                       [mean](long double x) { return x - mean; });

        long double sq_sum =
            std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        long double variance = sq_sum / data.size();
        return variance;
    }

    template <typename T>
    bool read_object_data(std ::string const &filename,
                          std::vector<Object> *data)
    {
        is_valid_file(filename);
        std::cout << "Reading from " << filename << std::endl;
        std::fstream in(filename);
        std::string line;
        int obj_id;
        T lat, lon;

        std::unordered_map<int, std::vector<Point>> object_map;

        while (std::getline(in, line))
        {
            std::stringstream ss(line);
            if (!(ss >> obj_id >> lat >> lon))
                break;
            object_map[obj_id].push_back(Point{lat, lon});
        }

        for (auto &obj : object_map)
        {
            std::vector<Point> obj_points;
            for (auto &i : obj.second)
            {
                obj_points.push_back(Point{i.lat, i.lon});
            }
            Object temp_obj(obj_points);
            temp_obj.setId(obj.first);
            data->push_back(temp_obj);
        }

        return true;
    }

    // template <typename T>
    // bool read_data(std ::string const &filename, std::vector<Point> *data)
    // {
    //     is_valid_file(filename);
    //     std::cout << "Reading from " << filename << std::endl;
    //     std::fstream in(filename);
    //     std::string line;
    //     T lat, lon;
    //     std::vector<T> lats, lons;

    //     while (std::getline(in, line))
    //     {
    //         std::stringstream ss(line);
    //         if (!(ss >> lat >> lon))
    //             break;
    //         data->push_back(Point{lat, lon});
    //         lats.push_back(lat);
    //         lons.push_back(lon);
    //     }

    //     long double lat_var = getVariance(lats);
    //     long double lon_var = getVariance(lons);
    //     std::cout << "lat_var: " << lat_var << " lon_var" << lon_var
    //               << std::endl;

    //     // Check if # unique value of latitude is larger than # unique value
    //     of
    //     // lontitude
    //     bool sort_by_lat = (lat_var <= lon_var) ? true : false;
    //     sort_data(sort_by_lat, data);
    //     remove_repeated(data);
    //     reset_id(data);

    //     return sort_by_lat;
    // }

    template <typename T>
    bool read_data(std ::string const &filename, std::vector<Point> *data)
    {
        is_valid_file(filename);
        std::cout << "Reading from " << filename << std::endl;
        std::fstream in(filename);
        std::string line;
        std::set<T> lat_counter;
        std::set<T> lon_counter;
        T lat, lon;

        while (std::getline(in, line))
        {
            std::stringstream ss(line);
            if (!(ss >> lat >> lon))
                break;
            data->push_back(Point{lat, lon});
            lat_counter.insert(lat);
            lon_counter.insert(lon);
        }

        // Check if # unique value of latitude is larger than # unique value of
        // lontitude
        bool sort_by_lat =
            (lat_counter.size() >= lon_counter.size()) ? true : false;
        sort_data(sort_by_lat, data);
        remove_repeated(data);
        reset_id(data);

        return sort_by_lat;
    }

}; // namespace utils

#endif
