#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <sys/stat.h>

#include "../include/entry.h"
#include "../include/learnedhashmap.h"
#include "../include/linear.h"
#include "../include/piecewise.h"
#include "../include/utils.h"

typedef std::vector<long double> DataVec;
typedef model::Linear<long double> LinearModel;
typedef model::Piecewise<long double> PiecewiseModel;

int main(int argc, char *argv[])
{
    /*
     *
     * Reading data from file
     *
     */
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " input file missing" << std::endl;
        return 1;
    }

    std::vector<Point> data;
    bool sort_by_lat = utils::read_data<long double>(argv[1], &data);
    std::string sort_text =
        sort_by_lat ? std::string("lat") : std::string("lon");

    std::cout << "\n-READ DATA\nData Stats:\n"
              << "  Sort by " << sort_text << "\n"
              << "  Size: " << data.size() << std::endl;

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
    DataVec lats;
    DataVec lons;
    DataVec test_set;
    DataVec train_y;
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
    DataVec train_x = sort_by_lat ? lats : lons;

    /*
     *
     * LearnedHashMap
     *
     *    Building LearnedHashMap using hash key that from model predicted
    value.
     *    Size of the table is (MAX_PRED_VALUE - MIN_PRED_VALUE + 1).
     *    Every predicted value have to subtract by the MIN_PRED_VALUE to get
    rid
     *    of negative value and keep every index within size of the table.
     *
     */
    std::cout << "\n-BUILD HASHMAP" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    LearnedHashMap<int, Point, LinearModel> hashmap(sort_by_lat, train_x,
                                                    train_y);
    for (i = 0; i < data.size(); i++)
        hashmap.insertNode(data[i]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "LearnedHashMap insertion time: " << duration.count()
              << " nanoseconds\nHashmap Stats:";
    bool full_info = false;
    hashmap.display_stats(full_info);
    // hashmap.display();

    /*
     *
     * Test look up performance
     *
     */
    // First look up loop for recording performance
    Point tmp_result;
    start = std::chrono::high_resolution_clock::now();

    for (i = 0; i < test_set.size(); i += 2)
        hashmap.getNode(test_set[i], test_set[i + 1], tmp_result);

    end = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    // Second look up loop for testing accuracy
    int found = 0;
    std::vector<Point> lookup_results;
    for (i = 0; i < test_set.size(); i += 2)
    {
        bool tmp_found =
            hashmap.getNode(test_set[i], test_set[i + 1], tmp_result);
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
    std::cout << "\n-RESULTS\nLearnedHashMap look up results:\n  Found: "
              << found << "\n  True Positives: " << true_positives
              << "\n  Precision: "
              << true_positives / (double)data.size() * 100.0 << "% "
              << "\n  Recall: " << true_positives / (double)found * 100.0
              << "% "
              << "\n  Look up time: " << duration.count() << " nanoseconds"
              << "\n  Average look up time: " << duration.count() / data.size()
              << " nanoseconds" << std::endl;

    /*
     *
     * Range query
     *
     */
    std::cout << "\n-RANGE QUERY";

    long double min[2] = {0.395, 0.398};
    long double max[2] = {0.4, 0.41};
    std::cout << "\n  Search range: {min_point: (" << min[0] << "," << min[1]
              << "), max_point: (" << max[0] << "," << max[1] << ")}";

    std::vector<Point> result;
    bool found_result = hashmap.rangeSearch(min, max, &result);

    if (found_result)
    {
        std::cout << "  Search result: " << result.size() << std::endl;
        for (size_t i = 0; i < result.size(); i++)
        {
            std::cout << "    " << result[i].id << " " << result[i].lat << " "
                      << result[i].lon << std::endl;
        }
    }
    else
    {
        std::cout << " No results found" << std::endl;
    }

    /*
     *
     * Line segments
     *
     */
    std::cout << "\n-Objects(line segments, retangles)";

    Object line(0, 0, 1, 1);

    std::cout << "\nBBOX: " << std::get<0>(line.getBbox())
              << std::get<1>(line.getBbox()) << std::get<2>(line.getBbox())
              << std::get<3>(line.getBbox())
              << "Centroid: " << line.getCentroid().first << " "
              << line.getCentroid().second << std::endl;

    return 0;
}
