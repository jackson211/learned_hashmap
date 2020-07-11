#pragma once

#ifndef ENTRY_H
#define ENTRY_H

#include <cstdint>
#include <iostream>
#include <tuple>
#include <vector>

typedef uint64_t id_type;

struct Point
{
    long double lat, lon;
    id_type id;

    void setId(id_type i) { id = i; }
};

const bool operator==(const Point &lhs, const Point &rhs)
{
    return (lhs.lat == rhs.lat && lhs.lon == rhs.lon);
}

class Object
{
public:
    Object(std::vector<Point> points)
    {
        long double min_x = points[0].lat;
        long double min_y = points[0].lon;
        long double max_x = points[0].lat;
        long double max_y = points[0].lon;

        for (auto &p : points)
        {
            if (p.lat < min_x)
                min_x = p.lat;
            else if (p.lat > max_x)
                max_x = p.lat;
            if (p.lon < min_y)
                min_y = p.lon;
            else if (p.lon > max_y)
                max_y = p.lon;
        }

        bbox = std::make_tuple(Point{min_x, min_y}, Point{max_x, max_y});
        centroid = Point{(max_x - min_x) / 2.0, (max_y - min_y) / 2.0};
    }

    Object(const long double &min_x, const long double &min_y,
           const long double &max_x, const long double &max_y)
        : bbox(Point{min_x, min_y}, Point{max_x, max_y})
    {
        checkBounds(min_x, min_y, max_x, max_y);
        centroid = Point{(max_x - min_x) / 2.0, (max_y - min_y) / 2.0};
    }

    ~Object() = default;

    void checkBounds(const long double &min_x, const long double &min_y,
                     const long double &max_x, const long double &max_y)
    {
        assert(min_x <= max_x && min_y <= max_y);
    }

    std::tuple<Point, Point> getBbox() const { return bbox; }

    Point getCentroid() const { return centroid; }

    id_type getId() const { return id; }

    void setId(id_type i) { id = i; }

private:
    std::tuple<Point, Point> bbox;
    Point centroid;
    id_type id;
};

#endif
