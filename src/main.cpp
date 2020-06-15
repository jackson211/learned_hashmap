#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "../include/entry.h"
#include "../include/linear.h"
#include "../include/utils.h"

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

std::unordered_map<int, std::vector<int>>
build_index_map(std::vector<int> &input) {
  std::cout << "Build index map..." << std::endl;
  std::unordered_map<int, std::vector<int>> index_map;
  for (size_t i = 0; i < input.size(); i++) {
    index_map[input[i]].push_back(i);
  }

  for (auto &item : index_map) {
    std::cout << item.first << " -> ";
    for (auto &i : item.second) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }

  return index_map;
}

template <typename Model, typename PredicateType = int>
std::vector<int> look_up(std::vector<Entry *> &data, Model &model,
                         std::unordered_map<int, std::vector<int>> &index_map,
                         std::vector<long double> &test_set) {
  std::cout << "Look up for test set..." << std::endl;
  std::vector<int> test_result;

  auto start = std::chrono::high_resolution_clock::now();

  for (size_t k = 0; k < test_set.size(); k += 2) {
    const long double test_lat = test_set[k];
    const long double test_lon = test_set[k + 1];
    int index = model.template predict<PredicateType>(test_lat);
    int found = -1; // -1: not found
    for (auto &i : index_map[index]) {
      if (data.at(i)->getLon() == test_lon) {
        if (data.at(i)->getLat() == test_lat) {
          found = i;
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

int main(int argc, char *argv[]) {
  // Reading data from file
  std::string filename = "test2.txt";
  std::vector<Entry *> data;
  bool sort_by_lat = utils::read_data<long double>(filename, &data);
  std::cout << "Size of data: " << sizeof(data.size()) << std::endl;

  // Spilt data into latitudes and longtitudes and train_y which is the index
  std::vector<long double> lats;
  std::vector<long double> lons;
  std::vector<long double> train_y;
  for (size_t i = 0; i < data.size(); i++) {
    lats.push_back(data.at(i)->getLat());
    lons.push_back(data.at(i)->getLon());
    train_y.push_back(i);
  }

  // Train model
  std::vector<long double> train_x = sort_by_lat ? lats : lons;
  model::Linear<long double> linear_model;
  linear_model.fit(train_x, train_y);

  // Prediction
  std::vector<int> pred_result = linear_model.predict_list<int>(train_x);
  const int MAX_PRED_VALUE =
      *std::max_element(pred_result.begin(), pred_result.end());
  const int MIN_PRED_VALUE =
      *std::min_element(pred_result.begin(), pred_result.end());
  std::cout << "Max: " << MAX_PRED_VALUE << ", Min: " << MIN_PRED_VALUE
            << std::endl;
  // for (size_t i = 0; i < pred_result.size(); i++) {
  // pred_result[i] -= MIN_PRED_VALUE;
  //}
  // std::unordered_map<int, int> index_freq = countFreq(pred_result);

  // Build test set
  std::vector<long double> test_set;
  for (size_t j = 0; j < data.size(); j++) {
    test_set.push_back(data.at(j)->getLat());
    test_set.push_back(data.at(j)->getLon());
  }
  test_set.push_back(0);
  test_set.push_back(0);

  std::vector<int> test_y;
  for (size_t i = 0; i < train_y.size(); i++) {
    test_y.push_back(i);
  }
  test_y.push_back(-1);

  // Build indexes map
  std::unordered_map<int, std::vector<int>> index_map =
      build_index_map(pred_result);

  std::vector<int> test_result = look_up<model::Linear<long double>>(
      data, linear_model, index_map, test_set);

  int correct = 0;
  for (int i = 0; i < test_result.size(); i++) {
    assert(test_result[i] == test_y[i]);
    correct++;
  }
  std::cout << "Correct: " << correct << std::endl;

  return 0;
}
