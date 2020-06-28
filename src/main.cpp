#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <sys/stat.h>

#include "../include/entry.h"
#include "../include/hashmap.h"
#include "../include/linear.h"
#include "../include/piecewise.h"
#include "../include/utils.h"

typedef std::vector<long double> DataType;
typedef model::Linear<long double> LinearModel;
typedef model::Piecewise<long double> PiecewiseModel;

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " input file missing" << std::endl;
    return 1;
  }

  /*
   *
   * Reading data from file
   *
   */
  std::vector<Entry> data;
  bool sort_by_lat = utils::read_data<long double>(argv[1], &data);
  std::string sort_text = sort_by_lat ? std::string("lat") : std::string("lon");
  std::cout << "Sort by " << sort_text << "\n"
            << "Size: " << data.size() << std::endl;

  /*
   *
   * Setting up train and test data
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
  for (i = 0; i < data.size(); i++)
  {
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
   * Training linear model
   *
   *    train_x is decided by variance of latitudes or longitudes which we wish
   *    to train on the column that has more spreaded data distribution.
   *
   */
  DataType train_x = sort_by_lat ? lats : lons;
  LinearModel lm;
  std::cout << "Training on "
            << "Linear model" << std::endl;

  // Training Timing start

  auto start = std::chrono::high_resolution_clock::now();
  lm.fit(train_x, train_y);
  auto end = std::chrono::high_resolution_clock::now();

  // Training Timing end

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "Training time: " << duration.count() << " nanoseconds"
            << std::endl;
  std::cout << "Slope: " << lm.getSlope()
            << ", Intercept: " << lm.getIntercept() << std::endl;

  /*
   *
   * Piecewise Model
   *
   */
  // PiecewiseModel pm;
  // pm.fit(train_x, train_y);
  // std::vector<int> pred_result = pm.predict_list<int>(train_x);

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
            << ", Max-Min: " << MAX_PRED_VALUE - MIN_PRED_VALUE + 1 << std::endl;

  /*
   *
   * HashMap
   *
   *    Building HashMap using hash key that from model predicted value.
   *    Size of the table is (MAX_PRED_VALUE - MIN_PRED_VALUE + 1).
   *    Every predicted value have to subtract by the MIN_PRED_VALUE to get
  rid
   *    of negative value and keep every index within size of the table.
   *
   */
  std::cout << "Building HashMap..." << std::endl;

  HashMap<int, LinearModel> hashmap(lm, sort_by_lat, MIN_PRED_VALUE, MAX_PRED_VALUE);
  for (i = 0; i < data.size(); i++)
    hashmap.insertNode(data[i]);

  std::cout << "Inserted all nodes into HashMap" << std::endl;
  std::cout << "Hashmap Stats:" << std::endl;
  bool full_info = false;
  hashmap.display_stats(full_info);
  // hashmap.display();

  /*
   *
   * Test look up performance
   *
   */
  Entry tmp_result;

  // First look up loop for recording performance
  start = std::chrono::high_resolution_clock::now();
  for (i = 0; i < test_set.size(); i += 2)
  {
    hashmap.getNode(test_set[i], test_set[i + 1], tmp_result);
  }
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  // Second look up loop for testing accuracy
  int found = 0;
  std::vector<Entry> lookup_results;
  for (i = 0; i < test_set.size(); i += 2)
  {
    bool tmp_found = hashmap.getNode(test_set[i], test_set[i + 1], tmp_result);
    if (tmp_found)
    {
      found++;
      lookup_results.push_back(tmp_result);
    }
  }

  int true_positives = 0;
  for (i = 0; i < lookup_results.size(); i++)
  {
    if (lookup_results[i] == data[i])
      true_positives++;
  }

  /*
   *
   * Print stats
   *
   */
  std::cout << "-------------------------\nHashMap look up results:\nFound :"
            << found << "\nTrue Positives: " << true_positives
            << "\nPrecision: " << true_positives / (double)data.size() * 100.0
            << "% "
            << "\nRecall: " << true_positives / (double)found * 100.0 << "% "
            << "\nLook up time: " << duration.count() << " nanoseconds"
            << "\nAverage look up time: " << duration.count() / data.size()
            << " nanoseconds" << std::endl;

  return 0;
}
