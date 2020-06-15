#pragma once

#ifndef ENTRY_H
#define ENTRY_H

#include <cstdint>

typedef uint64_t id_type;

class Entry {
public:
  Entry(const long double &lat, const long double &lon);
  ~Entry() = default;
  id_type getId();
  void setId(id_type id);
  const long double getLat();
  const long double getLon();
  bool isEqual(Entry &other) const;

private:
  id_type _id;
  const long double _lat;
  const long double _lon;
};

#endif
