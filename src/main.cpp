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

  // Reading data from file
  std::vector<Entry> data;
  bool sort_by_lat = utils::read_data<long double>(filename, &data);
  std::string sort_text = sort_by_lat ? std::string("lat") : std::string("lon");
  std::cout << "Sort by " << sort_text << "\n"
            << "Size: " << data.size() << std::endl;

  /*
   *
   * Spilt train and test data
   *
   *  lats: only contains latitudes
   *  lons: only contains longitudes
   *  test_set: contains both latitudes and longitudes in continous order
   *  train_y: vector of long double of label values
   *
   */

  DataType lats;
  DataType lons;
  DataType test_set;
  DataType train_y;
  int i;
  for (i = 0; i < data.size(); i++) {
    long double lat = data[i].lat;
    long double lon = data[i].lon;
    lats.push_back(lat);
    lons.push_back(lon);
    test_set.push_back(lat);
    test_set.push_back(lon);
    train_y.push_back(i);
  }

  /*
   *
   * Train linear model
   *
   *    train_x is decided by variance of latitudes or longitudes which we wish
   *    to train on the column that has more spreaded data distribution.
   *
   */
  DataType train_x = sort_by_lat ? lats : lons;
  LinearModel lm;

  // Training Timing start

  auto start = std::chrono::high_resolution_clock::now();
  lm.fit(train_x, train_y);
  auto end = std::chrono::high_resolution_clock::now();

  // Training Timing end

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "Training time: " << duration.count() << " nanoseconds"
            << std::endl;

  /*
   *
   * Prediction
   *
   *    Predicted value in default is int.
   *    Maximum and minimum predicted value is required to calculate the range
   *    of data indexes.
   *
   */
  std::vector<int> pred_result = lm.predict_list<int>(train_x);
  const int MAX_PRED_VALUE =
      *std::max_element(pred_result.begin(), pred_result.end());
  const int MIN_PRED_VALUE =
      *std::min_element(pred_result.begin(), pred_result.end());
  std::cout << "Max: " << MAX_PRED_VALUE << ", Min: " << MIN_PRED_VALUE
            << ", Max-Min: " << MAX_PRED_VALUE - MIN_PRED_VALUE << std::endl;

  // PiecewiseModel pm;
  // pm.fit(train_x, train_y);
  // for (i = 0; i < lats.size(); i++) {
  //   int p = pm.predict<int>(lats[i]);
  //   std::cout << "Input: " << lats[i] << " prediction: " << p << std::endl;
  // }

  /*
   *
   * HashMap
   *
   *    Building HashMap using hash key that from model predicted value.
   *    Size of the table is (MAX_PRED_VALUE - MIN_PRED_VALUE + 1).
   *    Every predicted value have to subtract by the MIN_PRED_VALUE to get rid
   *    of negative value and keep every index within size of the table.
   *
   */
  std::cout << "Building HashMap..." << std::endl;
  HashMap<int, LinearModel> hm(lm, MAX_PRED_VALUE - MIN_PRED_VALUE + 1,
                               sort_by_lat, MIN_PRED_VALUE);
  for (i = 0; i < data.size(); i++)
    hm.insertNode(data[i]);
  std::cout << "Inserted all nodes into HashMap" << std::endl;

  /*
   *
   * Test look up performance
   *
   */
  std::vector<Entry> lookup_results;
  Entry tmp_result;
  int found = 0;

  // Look up Timing start

  start = std::chrono::high_resolution_clock::now();
  for (i = 0; i < test_set.size(); i += 2) {
    found += hm.getNode(test_set[i], test_set[i + 1], tmp_result);
  }
  end = std::chrono::high_resolution_clock::now();

  // Look up Timing end

  for (i = 0; i < test_set.size(); i += 2) {
    bool tmp_found = hm.getNode(test_set[i], test_set[i + 1], tmp_result);
    lookup_results.push_back(tmp_result);
  }
  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  int true_positives = 0;
  for (i = 0; i < lookup_results.size(); i++) {
    if (lookup_results[i] == data[i]) {
      true_positives++;
    }
  }

  /*
   *
   * Print results
   *
   */
  std::cout << "--------------------\nHashMap look up results:\nFound :"
            << found << "\nTrue Positives: " << true_positives
            << "\nPrecision: " << true_positives / (double)data.size() * 100.0
            << "% "
            << "\nRecall: " << true_positives / (double)found * 100.0 << "% "
            << "\nLook up time: " << duration.count() << " nanoseconds"
            << "\nAverage look up time: " << duration.count() / data.size()
            << " nanoseconds" << std::endl;

  return 0;
}
