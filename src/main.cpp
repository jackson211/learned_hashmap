#include <set>
#include <algorithm>
#include <string>
#include <assert.h>
#include <iostream>
#include <unordered_map>

#include "HashMap.h"
#include "Linear.h"
#include "utils.h"
#include "unordered_map_hash.h"

template <typename T>
size_t getHashMapSize(T &list)
{
    std::set<size_t> counter;
    for (int i = 0; i < list.size(); i++)
    {
        counter.insert(list.at(i));
    }
    std::cout << "Hash Map Size: " << counter.size() << std::endl;
    return counter.size();
}

template <typename T>
void countFreq(std::vector<T> &list)
{
    std::unordered_map<T, int> mp;

    for (int i = 0; i < list.size(); i++)
        mp[list[i]]++;

    int total = 0;
    int count = 0;
    for (auto x : mp)
    {
        // std::cout << x.first << " " << x.second << std::endl;
        total += x.second;
        count++;
    }
    std::cout << "Total value: " << total << std::endl;
    std::cout << "Average: " << total / count << std::endl;
}

// auto timeFuncInvocation =
//     [](auto &&func, auto &&... params) {
//         // get time before function invocation
//         const auto &start = std::chrono::high_resolution_clock::now();
//         // function invocation using perfect forwarding
//         for (auto i = 0; i < 100000 /*largeNumber*/; ++i)
//         {
//             std::forward<decltype(func)>(func)(std::forward<decltype(params)>(params)...);
//         }
//         // get time after function invocation
//         const auto &stop = std::chrono::high_resolution_clock::now();
//         return (stop - start) / 100000 /*largeNumber*/;
//     };

int main()
{
    // Read data
    std::string filename = "test2.txt";
    ModelData data = utils::read_data(filename, true);

    std::vector<long double> lats;
    std::vector<long double> lons;
    std::vector<long double> train_y;
    for (int i; i < data.size(); i++)
    {
        lats.push_back(data.list[i].lat);
        lons.push_back(data.list[i].lon);
        train_y.push_back(i);
    }

    // Train
    std::vector<long double> train_x = data.sortByLat ? lats : lons;
    Linear<long double> model;
    model.fit(train_x, train_y);

    // Prediction
    std::vector<int> pred_result = model.predict_list<int>(train_x);
    const int MAX_PRED_VALUE = *std::max_element(pred_result.begin(), pred_result.end());
    const int MIN_PRED_VALUE = *std::min_element(pred_result.begin(), pred_result.end());
    // std::cout << "Min: " << MIN_PRED_VALUE << std::endl;
    // size_t hsize = getHashMapSize<std::vector<int>>(pred_result);
    countFreq<int>(pred_result);
    std::cout << "Prediction: " << std::endl;
    for (size_t i = 0; i < pred_result.size(); i++)
    {
        pred_result[i] = pred_result[i] - MIN_PRED_VALUE;
    }

    // test set
    std::vector<long double> test_set;
    for (int k = 0; k < data.list.size(); k++)
    {
        test_set.push_back(data.list[k].lat);
        test_set.push_back(data.list[k].lon);
    }

    Unordered_map_hash<int, Linear<long double>> umap(data, model, MIN_PRED_VALUE);
    umap.test(test_set);

    // put into hashmap
    HashMap<long, 100, Linear<long double>> hmap(data, model, MIN_PRED_VALUE);
    for (int j = 0; j < data.list.size(); j++)
    {
        hmap.put(data.list[j]);
    }

    hmap.test(test_set);
}
