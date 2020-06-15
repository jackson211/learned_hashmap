#include "../include/entry.h"

Entry::Entry(const long double &lat, const long double &lon)
    : _lat(lat), _lon(lon) {}

id_type Entry::getId() { return _id; }

void Entry::setId(id_type id) { _id = id; }

const long double Entry::getLat() { return _lat; }

const long double Entry::getLon() { return _lon; }

bool Entry::isEqual(Entry &other) const {
  return (_id == other.getId()) && (_lat == other.getLat()) &&
         (_lon == other.getLon());
}
