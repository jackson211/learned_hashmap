#pragma once

#include "utils.h"

template <typename T>
class Linear
{
private:
    T _slope, _intercept;

public:
    T getSlope() const
    {
        return _slope;
    }

    T getIntercept() const
    {
        return _intercept;
    }

    void fit(const ModelData &data)
    {
        T sum_x = 0;
        T sum_y = 0;
        int n = 0;

        // Calculate mean
        for (int i = 0; i < data.list.size(); i++)
        {
            n += 1;
            T x = data.list[i].lat;
            T y = i;
            sum_x += x;
            sum_y += y;
        }

        T mean_x = sum_x / n;
        T mean_y = sum_y / n;
        T sq_diff_sum = 0;
        T cov_diff_sum = 0;

        // Calculate Covariance and variance
        for (int j = 0; j < data.list.size(); j++)
        {
            T x = data.list[j].lat;
            T y = j;
            T x_diff = x - mean_x;
            T y_diff = y - mean_y;
            sq_diff_sum += x_diff * x_diff;
            cov_diff_sum += x_diff * y_diff;
        }
        T var = sq_diff_sum / (n - 1);
        T cov = cov_diff_sum / (n - 1);

        // Get the slope and intercept
        _slope = cov / var;
        _intercept = mean_y - mean_x * _slope;
        std::cout << "slope: " << _slope << ", intercept: " << _intercept << std::endl;
    }

    T predict(T x)
    {
        return x * _slope + _intercept;
    }

    template <typename V>
    std::vector<V> predict_list(std::vector<T> test_x)
    {
        std::vector<V> result;
        for (int i = 0; i < test_x.size(); i++)
        {
            V pred_y = predict(test_x[i]);
            result.push_back(pred_y);
        }
        return result;
    }
};