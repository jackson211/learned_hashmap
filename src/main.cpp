#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <sys/stat.h>
#include <unordered_map>

#include "../include/HashMap.h"
#include "../include/entry.h"
#include "../include/index.h"
#include "../include/linear.h"
#include "../include/piecewise.h"
#include "../include/utils.h"

typedef std::vector<long double> DataType;
typedef model::Linear<long double> LinearModel;
typedef model::Piecewise<long double> PiecewiseModel;

// Build indexes map
// std::unordered_map<int, std::vector<Entry>> index_map =
//     learned_index::build_index_map(pred_result, data);

// std::vector<int> indexes_lookup_result =
//     learned_index::look_up<LinearModel>(lm, index_map, test_set);

// std::cout << "Indexes Map look up result size: "
//           << indexes_lookup_result.size() << "\n"
//           << "True label size: " << true_y.size() << std::endl;

// int correct = 0;
// for (i = 0; i < indexes_lookup_result.size(); i++) {
//   if (indexes_lookup_result[i] == true_y[i])
//     correct++;
// }

// // Print result
// // learned_index::print_index_map<std::unordered_map<int,
// // std::vector<Entry>>>(
// //   index_map);

// std::cout << "Correct: " << correct << std::endl;
// std::cout << "Number of test label: " << indexes_lookup_result.size()
//           << std::endl;
// std::cout << "Number of true label: " << true_y.size() << std::endl;
// std::cout << "Correctness: " << correct / (double)true_y.size() * 100.0
//           << "% " << std::endl;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " input file missing" << std::endl;
    return 1;
  }

  // Check is file readable
  std::string filename = argv[1];
  struct stat buffer;
  if (stat(filename.c_str(), &buffer) != 0) {
    std::cout << "Error reading " << filename << std::endl;
    exit(1);
  };
  std::cout << "Reading from " << filename << std::endl;

  // Reading data from file
  std::vector<Entry> data;
  bool sort_by_lat = utils::read_data<long double>(filename, &data);
  std::string sort_text = sort_by_lat ? std::string("lat") : std::string("lon");
  std::cout << "Sort by " << sort_text << "\n"
            << "Size: " << data.size() << std::endl;

  // Spilt data into latitudes and longtitudes and train_y which is the index
  DataType lats;
  DataType lons;
  DataType train_y;
  int i;
  for (i = 0; i < data.size(); i++) {
    lats.push_back(data.at(i).lat);
    lons.push_back(data.at(i).lon);
    train_y.push_back(i);
  }

  // Train model
  DataType train_x = sort_by_lat ? lats : lons;
  LinearModel lm;

  // Timing start
  auto start = std::chrono::high_resolution_clock::now();
  lm.fit(train_x, train_y);
  // Timing end
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "Training time: " << duration.count() << " nanoseconds"
            << std::endl;

  // PiecewiseModel pm;
  // pm.fit(train_x, train_y);
  // for (i = 0; i < lats.size(); i++) {
  //   long double p = pm.predict<long double>(lats[i]);
  //   std::cout << "Input: " << lats[i] << " prediction: " << p << std::endl;
  // }

  // Prediction
  std::vector<int> pred_result = lm.predict_list<int>(train_x);
  const int MAX_PRED_VALUE =
      *std::max_element(pred_result.begin(), pred_result.end());
  const int MIN_PRED_VALUE =
      *std::min_element(pred_result.begin(), pred_result.end());
  std::cout << "Max: " << MAX_PRED_VALUE << ", Min: " << MIN_PRED_VALUE
            << std::endl;

  // Build test set
  DataType test_set;
  std::vector<int> true_y;
  for (i = 0; i < data.size(); i++) {
    test_set.push_back(data.at(i).lat);
    test_set.push_back(data.at(i).lon);
    true_y.push_back(i);
  }

  // HashMap
  std::cout << "Building HashMap..." << std::endl;
  HashMap<int, LinearModel> hm(lm, data.size(), sort_by_lat, MIN_PRED_VALUE);
  for (i = 0; i < data.size(); i++)
    hm.insertNode(data[i]);

  std::vector<Entry> lookup_results;
  int found_count = 0;
  // Timing start
  start = std::chrono::high_resolution_clock::now();
  for (i = 0; i < test_set.size(); i += 2) {
    Entry result;
    bool found = hm.getNode(test_set[i], test_set[i + 1], result);
    found_count += found;
    lookup_results.push_back(result);
  }
  // Timing end
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  int hm_correct = 0;
  for (i = 0; i < lookup_results.size(); i++) {
    if (lookup_results[i] == data[i]) {
      hm_correct++;
    }
  }

  std::cout << "HashMap results:\nFound :" << found_count << "\n"
            << "Correct: " << hm_correct << std::endl;
  std::cout << "Look up time: " << duration.count() << " nanoseconds"
            << std::endl;
  std::cout << "Average look up time: " << duration.count() / test_set.size()
            << " nanoseconds" << std::endl;
  std::cout << "Correctness: " << hm_correct / (double)true_y.size() * 100.0
            << "% " << std::endl;

  return 0;
}
