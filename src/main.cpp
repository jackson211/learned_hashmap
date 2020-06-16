#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "../include/entry.h"
#include "../include/index.h"
#include "../include/linear.h"
#include "../include/utils.h"

int main(int argc, char *argv[]) {
  // Reading data from file
  std::string filename = "test3.txt";
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
  std::unordered_map<int, std::vector<Entry *>> index_map =
      learned_index::build_index_map(pred_result, data);

  std::vector<int> test_result =
      learned_index::look_up<model::Linear<long double>>(linear_model,
                                                         index_map, test_set);

  int correct = 0;
  for (int i = 0; i < test_result.size(); i++) {
    assert(test_result[i] == test_y[i]);
    correct++;
  }
  std::cout << "Correct: " << correct / test_result.size() * 100.0 << "% "
            << std::endl;

  return 0;
}
