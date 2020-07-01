#pragma once

#ifndef LINEAR_H
#define LINEAR_H

#include <iostream>
#include <numeric>
#include <omp.h>
#include <vector>

namespace model
{

    template <typename DataType>
    void linear_fn(const std::vector<DataType> &train_x,
                   const std::vector<DataType> &train_y, long double *slope,
                   long double *intercept)
    {
        int n = train_x.size();
        long double sum_x = 0.0;
        long double sum_y = 0.0;

        // Kahan summation algorithm to reduce the small floating-point
        // numerical error.
        // See:https://en.wikipedia.org/wiki/Kahan_summation_algorithm
        long double cx = 0.0;
        long double cy = 0.0;

        // Calculate mean
        int i = 0;
#pragma omp parallel for shared(train_x, train_y) reduction(+ : sum_x, sum_y, cx, cy)
        for (i = 0; i < n; i++)
        {
            long double x = train_x[i] - cx;
            long double y = train_y[i] - cy;
            long double tx = sum_x + x;
            long double ty = sum_y + y;
            cx = (tx - sum_x) - x;
            cy = (ty - sum_y) - y;
            sum_x = tx;
            sum_y = ty;
        }
        long double mean_x = sum_x / n;
        long double mean_y = sum_y / n;
        long double sq_diff_sum = 0;
        long double cov_diff_sum = 0;

        // Calculate Covariance and variance
#pragma omp parallel for shared(train_x, train_y) reduction(+:sq_diff_sum,cov_diff_sum)
        for (i = 0; i < n; i++)
        {
            long double temp_x = train_x[i];
            long double temp_y = train_y[i];
            sq_diff_sum += (temp_x - mean_x) * (temp_x - mean_x);
            cov_diff_sum += (temp_x - mean_x) * (temp_y - mean_y);
        }
        long double var = sq_diff_sum / (n - 1);
        long double cov = cov_diff_sum / (n - 1);
        // Get the slope and intercept
        *slope = cov / var;
        *intercept = mean_y - mean_x * *slope;
    } // namespace model

    template <typename T> class Linear
    {
    public:
        Linear() = default;

        Linear(const std::vector<T> &train_x, const std::vector<T> &train_y)
        {
            fit(train_x, train_y);
        }

        ~Linear() = default;

        T getSlope() const { return _slope; }

        T getIntercept() const { return _intercept; }

        void fit(const std::vector<T> &train_x, const std::vector<T> &train_y)
        {
            linear_fn<T>(train_x, train_y, &_slope, &_intercept);
            _intercept += 0.5f; // Round to nearest int - Precalculate
        }

        template <typename V = int> V predict(T x)
        {
            return x * _slope + _intercept;
        }

        template <typename V = int>
        std::vector<V> predict_list(std::vector<T> test_x)
        {
            std::vector<V> result;
            for (size_t i = 0; i < test_x.size(); i++)
            {
                result.push_back(predict<V>(test_x[i]));
            }
            return result;
        }

    private:
        long double _slope;
        long double _intercept;
    };

} // namespace model

#endif
