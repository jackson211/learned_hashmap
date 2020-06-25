#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "entry.h"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

namespace utils {

auto compare_entry = [](Entry const &lhs, Entry const &rhs) -> bool {
  return lhs.lon == rhs.lon && lhs.lat == rhs.lat;
};

void reset_id(std::vector<Entry> *data) {
  for (size_t i = 0; i < data->size(); i++) {
    id_type id = i;
    data->at(i).setId(id);
  }
}

template <typename T> void rm_repeat(std::vector<Entry *> *data) {
  std::set<T> s;
  unsigned size = data->size();
  for (unsigned i = 0; i < size; ++i)
    s.insert(data[i]);
  data->assign(s.begin(), s.end());
}

// void sort_data(std::vector<Entry *> *data, bool compare) {
//   std::sort(data->begin(), data->end(), compare);
// }

template <typename T>
bool read_data(std ::string const &filename, std::vector<Entry> *data) {
  std::fstream in(filename);
  std::string line;
  std::set<T> lat_counter;
  std::set<T> lon_counter;
  T lat;
  T lon;

  while (std::getline(in, line)) {
    std::stringstream ss(line);
    if (!(ss >> lat >> lon))
      break;
    data->push_back(Entry{lat, lon});
    lat_counter.insert(lat);
    lon_counter.insert(lon);
  }

  // Check if # unique value of latitude is larger than # unique value of
  // lontitude
  bool sort_by_lat = (lat_counter.size() >= lon_counter.size()) ? true : false;

  // Sort data
  sort_by_lat ? std::sort(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lat < rhs.lat;
                          })
              : std::sort(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lon < rhs.lon;
                          });

  // Remove repeated data
  data->erase(std::unique(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lon == rhs.lon && lhs.lat == rhs.lat;
                          }),
              data->end());

  // Reset id for data
  reset_id(data);
  std::cout << "Total number of data: " << data->size() << std::endl;
  return sort_by_lat;
}
}; // namespace utils

#endif
