#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "entry.h"
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace utils {
auto compare_lat = [](Entry *const &lhs, Entry *const &rhs) -> bool {
  return lhs->getLat() < rhs->getLat();
};

auto compare_lon = [](Entry *const &lhs, Entry *const &rhs) -> bool {
  return lhs->getLon() < rhs->getLon();
};

void reset_id(std::vector<Entry *> *data) {
  for (size_t i = 0; i < data->size(); i++) {
    id_type id = i;
    data->at(i)->setId(id);
  }
}

template <typename T>
bool read_data(std ::string const &filename, std::vector<Entry *> *data) {
  std::fstream in(filename);
  std::string line;
  // std::vector<Entry *> data;
  std::set<T> lat_counter;
  std::set<T> lon_counter;

  int total = 0;
  while (std::getline(in, line)) {
    T lat;
    T lon;
    std::stringstream ss(line);

    while (ss >> lat >> lon) {
      Entry *item = new Entry(lat, lon);
      data->push_back(item);
      lat_counter.insert(lat);
      lon_counter.insert(lat);
    }
    ++total;
  }

  // Check if # unique value of latitude is larger than # unique value of
  // lontitude
  bool sort_by_lat = (lat_counter.size() >= lon_counter.size()) ? true : false;

  // Sort data
  if (sort_by_lat)
    std::sort(data->begin(), data->end(), compare_lat);
  else
    std::sort(data->begin(), data->end(), compare_lon);

  reset_id(data);

  return sort_by_lat;
}

}; // namespace utils

#endif
