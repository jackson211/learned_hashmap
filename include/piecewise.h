#pragma once

#ifndef PIECEWISE_H
#define PIECEWISE_H

#include "linear.h"
#include <iostream>
#include <vector>

namespace model {

struct ModelParam {
  long double slope, intercept;
};

template <typename T> class Piecewise {
public:
  Piecewise(){};

  ~Piecewise() = default;

  void fit(const std::vector<T> &train_x, const std::vector<T> &train_y) {
    breaks.push_back(0);
    breaks.push_back(2);
    breaks.push_back(7);
    breaks.push_back(9);

    build_regression(train_x, train_y, breaks);
  }

  void build_regression(const std::vector<T> &train_x,
                        const std::vector<T> &train_y,
                        const std::vector<int> &breaks) {

    for (int i = 0; i < breaks.size() - 1; i++) {
      int break_start = breaks[i];
      int break_end = breaks[i + 1] + 1;

      auto segment_train_x = std::vector<T>(train_x.begin() + break_start,
                                            train_x.begin() + break_end);
      auto segment_train_y = std::vector<T>(train_y.begin() + break_start,
                                            train_y.begin() + break_end);
      const T max_v =
          *std::max_element(segment_train_x.begin(), segment_train_x.end());
      const T min_v =
          *std::min_element(segment_train_x.begin(), segment_train_x.end());

      bounds.push_back(std::make_pair(min_v, max_v));
      least_squares_fit(segment_train_x, segment_train_y);
    }
  }

  void least_squares_fit(std::vector<T> &segment_train_x,
                         std::vector<T> &segment_train_y) {
    T s_slope;
    T s_intercept;
    linear_fn(segment_train_x, segment_train_y, &s_slope, &s_intercept);
    params.push_back(ModelParam{s_slope, s_intercept});

    for (int i = 0; i < segment_train_x.size(); i++) {
      std::cout << segment_train_x[i] << " ";
      std::cout << segment_train_y[i] << " ";
    }
    std::cout << "Slope: " << s_slope << " Intercept: " << s_intercept
              << std::endl;
  }

  bool in_range(const T v, const T min_val, const T max_val) {
    return (v - min_val) <= (max_val - min_val);
  }

  template <typename V = int> V predict(T x) {
    int segment;
    for (int i = 0; i < bounds.size(); i++) {
      if (in_range(x, bounds[i].first, bounds[i].second)) {
        segment = i;
        break;
      }
    }
    return params[segment].slope * x + params[segment].intercept + 0.5f -
           (x < 0);
  }

  template <typename V = int>
  std::vector<V> predict_list(std::vector<T> test_x) {
    std::vector<V> result;
    for (size_t i = 0; i < test_x.size(); i++) {
      result.push_back(predict<V>(test_x[i]));
    }
    return result;
  }

private:
  std::vector<int> breaks;
  std::vector<std::pair<T, T>> bounds;
  std::vector<ModelParam> params;
};
} // namespace model

#endif
