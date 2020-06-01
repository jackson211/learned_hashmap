#include <algorithm>
#include <string>
#include <assert.h>

#include "utils.h"
#include "HashMap.h"
// #include <tuple>

void slr(const ModelData &data, long double *slope, long double *intercept)
{
    long double sum_x = 0;
    long double sum_y = 0;
    int n = 0;
    std::vector<DataItem> dataset = data.list;

    // Calculate mean
    for (int i = 0; i < dataset.size(); i++)
    {
        n += 1;
        long double x = dataset[i].lat;
        long double y = i;
        sum_x += x;
        sum_y += y;
    }

    long double mean_x = sum_x / n;
    long double mean_y = sum_y / n;
    long double sq_diff_sum = 0;
    long double cov_diff_sum = 0;

    // Calculate Covariance and variance
    for (int j = 0; j < dataset.size(); j++)
    {
        long double x = dataset[j].lat;
        long double y = j;
        long double x_diff = x - mean_x;
        long double y_diff = y - mean_y;
        sq_diff_sum += x_diff * x_diff;
        cov_diff_sum += x_diff * y_diff;
    }
    long double var = sq_diff_sum / (n - 1);
    long double cov = cov_diff_sum / (n - 1);
    long double w = cov / var;
    long double b = mean_y - mean_x * w;
    *slope = w;
    *intercept = b;
    std::cout << "slope: " << w << ", intercept: " << b << std::endl;
}

int predict(long double x, long double *slope, long double *intercept)
{
    return x * *slope + *intercept;
}

std::vector<int> predict_list(std::vector<long double> test_x, long double *slope, long double *intercept)
{
    std::vector<int> result;
    for (int i = 0; i < test_x.size(); i++)
    {
        int pred_y = predict(test_x[i], slope, intercept);
        result.push_back(pred_y);
    }
    return result;
}

struct MyKeyHash
{
    unsigned long operator()(const int &k) const
    {
        return k % 10;
    }
};

int main()
{
    std::string filename = "test.txt";
    ModelData data = utils::read_data(filename, true);
    long double slope, intercept;
    slr(data, &slope, &intercept);

    std::vector<long double> test_x;
    for (int i = 0; i < data.list.size(); i++)
        test_x.push_back(data.list[i].lat);

    std::vector<int> pred_result = predict_list(test_x, &slope, &intercept);
    for (int j = 0; j < data.list.size(); j++)
        std::cout << test_x[j] << " " << pred_result[j] << std::endl;

    HashMap<int, std::string, 10, MyKeyHash> hmap;
    hmap.put(1, "1");
    hmap.put(2, "2");
    hmap.put(3, "3");

    std::string value;
    bool result = hmap.get(2, value);
    assert(result);
    assert(value == "2");

    result = hmap.get(3, value);
    assert(result);
    assert(value == "3");

    hmap.remove(3);
    result = hmap.get(3, value);
    assert(!result);
}
