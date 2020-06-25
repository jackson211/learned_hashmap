#pragma once

#ifndef INDEX_H
#define INDEX_H

#include "entry.h"
#include <iostream>
#include <unordered_map>
#include <vector>

namespace learned_index {

template <typename T>
std::unordered_map<T, int> countFreq(std::vector<T> &list) {
  std::unordered_map<T, int> mp;

  for (int i = 0; i < list.size(); i++)
    mp[list[i]]++;

  int total = 0;
  int count = 0;
  for (auto x : mp) {
    std::cout << x.first << " " << x.second << std::endl;
    total += x.second;
    count++;
  }
  std::cout << "Total value: " << total << std::endl;
  std::cout << "Average: " << total / count << std::endl;

  return mp;
}

std::unordered_map<int, std::vector<Entry>>
build_index_map(std::vector<int> &input, std::vector<Entry> &data) {
  std::cout << "Build index map..." << std::endl;
  std::unordered_map<int, std::vector<Entry>> index_map;
  for (size_t i = 0; i < input.size(); i++) {
    index_map[input[i]].push_back(data[i]);
  }
  return index_map;
}

template <typename T> void print_index_map(T &index_map) {
  for (auto &item : index_map) {
    std::cout << item.first << " -> ";
    for (auto &i : item.second) {
      std::cout << i.id << " ";
    }
    std::cout << std::endl;
  }
}

template <typename Model, typename PredicateType = int>
std::vector<int> look_up(Model &model,
                         std::unordered_map<int, std::vector<Entry>> &index_map,
                         std::vector<long double> &test_set) {

  std::cout << "Look up for test set..." << std::endl;
  std::vector<int> test_result;

  // Timing look up speed
  auto start = std::chrono::high_resolution_clock::now();

  for (size_t k = 0; k < test_set.size(); k += 2) {
    const long double test_lat = test_set[k];
    const long double test_lon = test_set[k + 1];
    int index = model.template predict<PredicateType>(test_lat);
    int found = -1; // -1: not found
    for (auto &i : index_map[index]) {
      if (i.lon == test_lon) {
        if (i.lat == test_lat) {
          found = i.id;
          break;
        }
      }
    }
    test_result.push_back(found);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "Look up time: " << duration.count() << " nanoseconds"
            << std::endl;
  std::cout << "Average look up time: " << duration.count() / test_set.size()
            << " nanoseconds" << std::endl;

  return test_result;
}
} // namespace learned_index
#endif
