#pragma once

#ifndef UNORDER_MAP_HASH_H
#define UNORDER_MAP_HASH_H

#include <unordered_map>
#include <vector>
#include "utils.h"
#include "linear.h"

template <typename T, typename M>
class Unordered_map_hash
{
private:
    std::unordered_map<T, std::vector<Coord>> _map;
    M _model;
    const int MIN_PRED_VALUE;

public:
    Unordered_map_hash(ModelData &data, M &model, const int MIN_PRED_VALUE) : _model(model), MIN_PRED_VALUE(MIN_PRED_VALUE)
    {
        for (size_t i = 0; i < data.list.size(); i++)
        {
            int key = _model.predict(data.list.at(i).lat) - MIN_PRED_VALUE;

            if (_map.find(key) == _map.end())
            {
                std::vector<Coord> list = {data.list.at(i)};
                _map.insert(std::pair<int, std::vector<Coord>>(key, list));
            }
            else
            {
                _map[key].push_back(data.list.at(i));
            }
        }
    }

    void print_map()
    {
        // Print _map
        for (const std::pair<T, std::vector<Coord>> &tup : _map)
        {
            std::cout << "K: " << tup.first << std::endl;
            for (const Coord &c : tup.second)
            {
                std::cout << "V: " << c.id << " " << c.lat << c.lon << std::endl;
            }
        }
    }

    template <typename V>
    void test(const std::vector<V> test_set)
    {
        long double w = _model.getSlope();
        long double b = _model.getIntercept();
        int count = 0;

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < test_set.size(); i += 2)
        {
            int key = _model.predict(test_set.at(i)) - MIN_PRED_VALUE;

            if (_map.find(key) == _map.end())
            {
                std::cout << "Not found " << key << std::endl;
            }
            else
            {
                for (Coord &item : _map[key])
                {
                    if (item.lon == test_set.at(i + 1))
                    {
                        if (item.lat == test_set.at(i))
                        {
                            count++;
                        }
                    }
                }
            }
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        std::cout << "Total look up time : "
                  << duration.count() << " nanoseconds" << std::endl;
        std::cout << "Average look up time : "
                  << duration.count() / (test_set.size() / 2) << " nanoseconds" << std::endl;
        std::cout << "Correct item found: " << count << std::endl;
    }
}; // namespace unorder_map_hash

#endif