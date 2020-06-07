#include <set>
#include <algorithm>
#include <string>
#include <assert.h>
#include <chrono>
#include <unordered_map>

#include "HashMap.h"
#include "KeyHash.h"
#include "Linear.h"
#include "utils.h"

size_t getHashMapSize(std::vector<int> &list)
{
    std::set<int> counter;
    for (int i = 0; i < list.size(); i++)
    {
        counter.insert(list.at(i));
    }
    std::cout << "Hash Map Size: " << counter.size() << std::endl;
    return counter.size();
}

void countFreq(std::vector<int> &list)
{
    std::unordered_map<int, int> mp;

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
    const auto MAX_PRED_VALUE = *std::max_element(pred_result.begin(), pred_result.end());
    const auto MIN_PRED_VALUE = *std::min_element(pred_result.begin(), pred_result.end());
    size_t hsize = getHashMapSize(pred_result);
    countFreq(pred_result);

    // put into hashmap
    HashMap<long, 100> hmap(model.getSlope(), model.getIntercept() - MIN_PRED_VALUE);
    for (int j = 0; j < data.list.size(); j++)
    {
        hmap.put(data.list[j]);
    }

    hmap.printHashMap();

    // test result
    std::vector<long double> test;
    for (int k = 0; k < data.list.size(); k++)
    {
        DATAITEM value = data.list[k];
        test.push_back(value.lat);
        test.push_back(value.lon);
    }

    // Look up
    auto start = std::chrono::high_resolution_clock::now();

    for (int k = 0; k < test.size(); k += 2)
    {
        DATAITEM testValue;
        hmap.get(test[k], test[k + 1], testValue);
        assert(testValue.isEqual(data.list[k / 2]));
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << "Total look up time : "
              << duration.count() << " nanoseconds" << std::endl;
    std::cout << "Average look up time : "
              << duration.count() / data.list.size() << " nanoseconds" << std::endl;
    return 0;
}
