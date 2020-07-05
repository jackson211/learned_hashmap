#pragma once

#ifndef ENTRY_H
#define ENTRY_H

#include <cstdint>
#include <vector>

typedef uint64_t id_type;

struct Point
{
    long double lat, lon;
    id_type id;

    void setId(id_type i) { id = i; }

    bool operator==(const Point &t) const
    {
        return (this->lat == t.lat && this->lon == t.lon);
    }
};

class Object
{
public:
    Object(const long double &min_x, const long double &min_y,
           const long double &max_x, const long double &max_y)
        : bbox({min_x, min_y, max_x, max_y})
    {
        checkBounds(min_x, min_y, max_x, max_y);
        centroid =
            std::make_pair((std::get<0>(bbox) + std::get<2>(bbox)) / 2.0,
                           (std::get<1>(bbox) + std::get<3>(bbox)) / 2.0);
    }

    ~Object() = default;

    void checkBounds(const long double &min_x, const long double &min_y,
                     const long double &max_x, const long double &max_y)
    {
        assert(min_x <= max_x && min_y <= max_y);
    }

    std::tuple<long double, long double, long double, long double>
    getBbox() const
    {
        return bbox;
    }

    void setBbox(const long double &min_x, const long double &min_y,
                 const long double &max_x, const long double &max_y)
    {
        checkBounds(min_x, min_y, max_x, max_y);
        bbox = {min_x, min_y, max_x, max_y};
        centroid =
            std::make_pair((std::get<0>(bbox) + std::get<2>(bbox)) / 2.0,
                           (std::get<1>(bbox) + std::get<3>(bbox)) / 2.0);
    }

    std::pair<long double, long double> getCentroid() const { return centroid; }

    id_type getId() const { return id; }

    void setId(id_type i) { id = i; }

private:
    std::tuple<long double, long double, long double, long double> bbox;
    std::pair<long double, long double> centroid;
    id_type id;
};

#endif
