#pragma once

#ifndef LINEAR_H
#define LINEAR_H

#include <iostream>
#include <numeric>
#include <omp.h>
#include <vector>

namespace model {

template <typename DataType>
void linear_fn(const std::vector<DataType> &train_x,
               const std::vector<DataType> &train_y, long double *slope,
               long double *intercept) {
  int n = train_x.size();
  int i = 0;

  DataType sum_x = 0;
  DataType sum_y = 0;

  // Calculate mean
#pragma omp parallel for shared(train_x, train_y) reduction(+ : sum_x, sum_y)
  for (i = 0; i < train_x.size(); i++) {
    sum_x += train_x[i];
    sum_y += train_y[i];
  }

  DataType mean_x = sum_x / n;
  DataType mean_y = sum_y / n;
  DataType sq_diff_sum = 0;
  DataType cov_diff_sum = 0;

  // Calculate Covariance and variance
#pragma omp parallel for shared(train_x, train_y) reduction(+ : sq_diff_sum, cov_diff_sum)
  for (i = 0; i < train_x.size(); i++) {
    DataType temp_x = train_x[i];
    DataType temp_y = train_y[i];
    sq_diff_sum += (temp_x - mean_x) * (temp_x - mean_x);
    cov_diff_sum += (temp_x - mean_x) * (temp_y - mean_y);
  }

  DataType var = sq_diff_sum / (n - 1);
  DataType cov = cov_diff_sum / (n - 1);

  // Get the slope and intercept
  *slope = cov / var;
  *intercept = mean_y - mean_x * *slope;
}

template <typename T> class Linear {
public:
  Linear() = default;

  ~Linear() = default;

  T getSlope() const { return _slope; }

  T getIntercept() const { return _intercept; }

  void fit(const std::vector<T> &train_x, const std::vector<T> &train_y) {
    linear_fn<T>(train_x, train_y, &_slope, &_intercept);
    _intercept += 0.5f; // Round to nearest int - Precalculate
    std::cout << "Slope: " << _slope << ", Intercept: " << _intercept
              << std::endl;
  }

  template <typename V = int> V predict(T x) { return x * _slope + _intercept; }

  template <typename V = int>
  std::vector<V> predict_list(std::vector<T> test_x) {
    std::vector<V> result;
    for (size_t i = 0; i < test_x.size(); i++) {
      result.push_back(predict<V>(test_x[i]));
    }
    return result;
  }

private:
  T _slope;
  T _intercept;
};

} // namespace model

#endif
