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

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap<KeyType, ValueType, ModelType>
build_hashmap(const bool &sort_by_lat, const DataVec &train_x,
              const DataVec &train_y, const std::vector<Point> &data)
{
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

    LearnedHashMap<KeyType, ValueType, ModelType> hashmap(sort_by_lat, train_x,
                                                          train_y);
    for (int i = 0; i < data.size(); i++)
        hashmap.insertNode(data[i]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "LearnedHashMap insertion time: " << duration.count()
              << " nanoseconds\nHashmap Stats:";

    bool full_info = false;
    hashmap.display_stats(full_info);
    // hashmap.display();

    return hashmap;
}

void point_data_flow(std ::string const &filename)
{
    std::vector<Point> data;
    bool sort_by_lat = utils::read_data<long double>(filename, &data);
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

    // Building hashmap
    LearnedHashMap<int, Point, LinearModel> hashmap =
        build_hashmap<int, Point, LinearModel>(sort_by_lat, train_x, train_y,
                                               data);

    /*
     *
     * Test look up performance
     *
     */
    // First look up loop for recording performance
    Point tmp_result;
    auto start = std::chrono::high_resolution_clock::now();

    for (i = 0; i < test_set.size(); i += 2)
        hashmap.pointSearch(test_set[i], test_set[i + 1], tmp_result);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    // Second look up loop for testing accuracy
    int found = 0;
    std::vector<Point> lookup_results;
    for (i = 0; i < test_set.size(); i += 2)
    {
        bool tmp_found =
            hashmap.pointSearch(test_set[i], test_set[i + 1], tmp_result);
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
}

void object_data_flow(std::string const &filename)
{
    std::vector<Object> objects;
    utils::read_object_data<long double>(filename, &objects);

    std::set<long double> lat_counter;
    std::set<long double> lon_counter;
    std::vector<Point> points;
    std::vector<std::pair<Point, Point>> point_pairs;
    size_t i;
    for (i = 0; i < objects.size(); i++)
    {
        std::tuple<Point, Point> bbox = objects[i].getBbox();
        id_type obj_id = objects[i].getId();
        Point p1 = std::get<0>(bbox);
        Point p2 = std::get<1>(bbox);
        p1.setId(obj_id);
        p2.setId(obj_id);
        points.push_back(p1);
        points.push_back(p2);
        point_pairs.push_back(std::make_pair(p1, p2));

        lat_counter.insert(p1.lat);
        lon_counter.insert(p1.lon);
        lat_counter.insert(p2.lat);
        lon_counter.insert(p2.lon);
    }
    bool sort_by_lat =
        (lat_counter.size() >= lon_counter.size()) ? true : false;

    utils::sort_data(sort_by_lat, &points);
    // Print point pairs
    for (i = 0; i < point_pairs.size(); i++)
    {
        Point p1 = point_pairs[i].first;
        Point p2 = point_pairs[i].second;
        std::cout << p1.lat << " " << p1.lon << " " << p2.lat << " " << p2.lon
                  << std::endl;
    }

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
    for (i = 0; i < points.size(); i++)
    {
        long double lat = points[i].lat;
        long double lon = points[i].lon;
        lats.push_back(lat);
        lons.push_back(lon);
        test_set.push_back(lat);
        test_set.push_back(lon);
        train_y.push_back(i);
    }
    DataVec train_x = sort_by_lat ? lats : lons;

    // Building hashmap
    /* LearnedHashMap<int, std::pair<Point, Point>, LinearModel> obj_hashmap =
     */
    /*     build_hashmap<int, std::pair<Point, Point>, LinearModel>( */
    /*         sort_by_lat, train_x, train_y, point_pairs); */

    std::cout << "\n-BUILD HASHMAP" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    LearnedHashMap<int, std::pair<Point, Point>, LinearModel> hashmap(
        sort_by_lat, train_x, train_y);
    for (int i = 0; i < point_pairs.size(); i++)
        hashmap.insertNode(point_pairs[i]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "LearnedHashMap insertion time: " << duration.count()
              << " nanoseconds\nHashmap Stats:";

    bool full_info = true;
    hashmap.display_stats(full_info);

    /* Point tmp_point; */
    /* long double s_lat = 144.961866; */
    /* long double s_lon = -37.795042; */
    /* std::cout << "Searching point: " << s_lat << " " << s_lon << std::endl;
     */
    /* bool found_point = obj_hashmap.approximateSearch(s_lat, s_lon,
     * tmp_point); */
    /* std::cout << "Found point " << found_point << std::endl; */

    /* for (i = 0; i < objects.size(); i++) */
    /* { */
    /*     if (objects[i].getId() == 14) */
    /*     { */
    /*         std::tuple<Point, Point> box = objects[i].getBbox(); */
    /*         std::cout << "14 parkville: " << std::get<0>(box).lat */
    /*                   << std::get<0>(box).lon << ", " << std::get<1>(box).lat
     */
    /*                   << std::get<1>(box).lon << std::endl; */
    /*     } */
    /* } */
}

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

    // point_data_flow(argv[1]);

    object_data_flow(argv[1]);

    return 0;
}
