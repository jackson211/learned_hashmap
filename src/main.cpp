#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <sys/stat.h>

#include "../include/entry.h"
#include "../include/learnedhashmap.h"
#include "../include/linear.h"
#include "../include/utils.h"

typedef std::vector<long double> DataVec;
typedef model::Linear<long double> LinearModel;

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap<KeyType, ValueType, ModelType>
build_hashmap(const bool &sort_by_lat, const DataVec &train_x,
              const DataVec &train_y, const std::vector<ValueType> &data)
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

    return hashmap;
}

template <typename MapType>
void test_performance(std::vector<Point> &data,
                      std::vector<long double> &test_set, MapType &hashmap)
{

    /*
     *
     * Test look up performance
     *
     */
    // First look up loop for recording performance
    int i;
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
    std::cout << "\n-RESULTS\n look up results:\n  Found: " << found
              << "\n  True Positives: " << true_positives << "\n  Precision: "
              << true_positives / (double)data.size() * 100.0 << "% "
              << "\n  Recall: " << true_positives / (double)found * 100.0
              << "% "
              << "\n  Look up time: " << duration.count() << " nanoseconds"
              << "\n  Average look up time: " << duration.count() / data.size()
              << " nanoseconds" << std::endl;
}

template <typename MapType>
void test_nearest_neighbor(const Point &p, MapType &hashmap)
{
    // std::cout << "\n  Random Points: "
    //           << "rank " << p.id << " coordinates: " << p.lat << ", " <<
    //           p.lon;
    std::vector<Point> result;
    distance_function df = euclidean;
    hashmap.nearestNeighborSearch(p, result, euclidean);

    // for (int i = 0; i < result.size(); i++)
    //     std::cout << "  Found nearest neighbor: [" << result[i].id << ", "
    //               << result[i].lat << ", " << result[i].lon << "]" <<
    //               std::endl;
}

void point_data_flow(std::string const &filename)
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
    sort_by_lat = true;
    DataVec train_x = sort_by_lat ? lats : lons;

    auto max_lat = *std::max_element(lats.begin(), lats.end());
    auto min_lat = *std::min_element(lats.begin(), lats.end());
    auto max_lon = *std::max_element(lons.begin(), lons.end());
    auto min_lon = *std::min_element(lons.begin(), lons.end());
    auto lat_size = max_lat - min_lat;
    auto lon_size = max_lon - min_lon;
    std::cout << "  max lat: " << max_lat << "  min lat: " << min_lat
              << "  max lon: " << max_lon << "  min lon: " << min_lon
              << "  lat size: " << lat_size << "  lon size: " << lon_size
              << std::endl;

    // Building hashmap
    LearnedHashMap<int, Point, LinearModel> hashmap =
        build_hashmap<int, Point, LinearModel>(sort_by_lat, train_x, train_y,
                                               data);

    bool full_info = false;
    hashmap.display_stats(full_info);
    // hashmap.display();

    test_performance<LearnedHashMap<int, Point, LinearModel>>(data, test_set,
                                                              hashmap);

    /*
     *
     * Range query
     *
     */
    std::cout << "\n-RANGE QUERY" << std::endl;
    long double start_n = 0.5;
    long double end_n = 0.5;
    for (int i = 0; i < 10; i++)
    {
        long double step = i * 0.05;
        long double range_min = start_n - step;
        long double range_max = end_n + step;

        long double min[2] = {range_min, range_min};
        long double max[2] = {range_max, range_max};

        std::cout << "\n"
                  << range_min << " " << range_max << "\nPercentage: "
                  << ((range_min - range_max) * (range_min - range_max)) /
                         (lat_size * lon_size);

        std::vector<Point> result;

        // Record range query time
        auto start = std::chrono::high_resolution_clock::now();
        bool found_result = hashmap.rangeSearch(min, max, &result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        std::cout << "Range Query look up time: " << duration.count()
                  << " nanoseconds" << std::endl;

        if (found_result)
            std::cout << "  Search result size: " << result.size()
                      << "\n Average look up time:"
                      << duration.count() / result.size() << std::endl;
        else
            std::cout << " No results found" << std::endl;
    }

    // Nearest neighbour search
    std::cout << "\n-NEAREST NEIGHBOR QUERY" << std::endl;
    std::vector<Point> knn_test_data;
    size_t n_values = 1000;
    std::cout << " number of test points: " << n_values << std::endl;
    // select random Points from the dataset
    std::sample(data.begin(), data.end(), std::back_inserter(knn_test_data),
                n_values, std::mt19937{std::random_device{}()});

    // Test
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n_values; i++)
    {
        test_nearest_neighbor<LearnedHashMap<int, Point, LinearModel>>(
            knn_test_data[i], hashmap);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "  knn time: " << duration.count() << " nanoseconds"
              << "\n  Average look up time: " << duration.count() / n_values
              << " nanoseconds" << std::endl;
}

void object_data_flow(std::string const &filename)
{
    std::vector<Object> objects;
    utils::read_object_data<long double>(filename, &objects);

    /*
     *
     * Preprocessing data
     *
     *
     */
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
    LearnedHashMap<int, std::pair<Point, Point>, LinearModel> obj_hashmap =
        build_hashmap<int, std::pair<Point, Point>, LinearModel>(
            sort_by_lat, train_x, train_y, point_pairs);

    bool full_info = false;
    obj_hashmap.display_stats(full_info);

    // Lookup region
    std::pair<Point, Point> result_region;
    long double s_lat = 144.971628;
    long double s_lon = -37.803018;
    std::cout << "Searching point: " << s_lat << " " << s_lon << std::endl;

    bool found_region = obj_hashmap.regionSearch(s_lat, s_lon, result_region);
    std::cout << "Found region " << found_region << std::endl;

    if (found_region)
    {
        std::cout << "Result:\nID:" << result_region.first.id << " "
                  << result_region.first.lat << " " << result_region.first.lon
                  << " " << result_region.second.id << " "
                  << result_region.second.lat << result_region.second.lon
                  << std::endl;
    }
    else
    {
        std::cout << "Region not found" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    /*
     *
     * Reading data from file
     *
     */
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " [object type] [input file]\n"
                  << "Object type:\n\tp\tPoint type\n\to\tObject type\n"
                  << "Input file:\n\ttext file" << std::endl;
        return 1;
    }
    if (*argv[1] == 'p')
    {
        std::cout << "Object type: Point" << std::endl;
        point_data_flow(argv[2]);
    }
    else if (*argv[1] == 'o')
    {
        std::cout << "Object type: Object" << std::endl;
        object_data_flow(argv[2]);
    }
    else
    {
        std::cerr << "Usage: " << argv[0] << " [object type] [input file]\n"
                  << "Object type:\n\tp\tPoint type\n\to\tObject type\n"
                  << "Input file:\n\ttext file" << std::endl;
        return 1;
    }

    return 0;
}
