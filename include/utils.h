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
#include <vector>

namespace utils {

void is_valid_file(const std::string filename) {
  struct stat buffer;
  if (stat(filename.c_str(), &buffer) != 0) {
    std::cout << "Error reading " << filename << std::endl;
    exit(1);
  };
}

void reset_id(std::vector<Entry> *data) {
  for (size_t i = 0; i < data->size(); i++) {
    id_type id = i;
    data->at(i).setId(id);
  }
}

void remove_repeated(std::vector<Entry> *data) {
  data->erase(std::unique(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lon == rhs.lon && lhs.lat == rhs.lat;
                          }),
              data->end());
}

void sort_data(bool sort_by_lat, std::vector<Entry> *data) {
  sort_by_lat ? std::sort(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lat < rhs.lat;
                          })
              : std::sort(data->begin(), data->end(),
                          [](const Entry &lhs, const Entry &rhs) {
                            return lhs.lon < rhs.lon;
                          });
}

template <typename T>
bool read_data(std ::string const &filename, std::vector<Entry> *data) {
  is_valid_file(filename);
  std::cout << "Reading from " << filename << std::endl;
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
  sort_data(sort_by_lat, data);
  remove_repeated(data);
  reset_id(data);

  return sort_by_lat;
}
}; // namespace utils

#endif
