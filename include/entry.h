#pragma once

#ifndef ENTRY_H
#define ENTRY_H

#include <cmath>
#include <cstdint>
#include <iostream>
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
    Object(std::vector<Point> points) : coordinates(points)
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

        bbox = std::make_pair(Point{min_x, min_y}, Point{max_x, max_y});
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

    std::vector<Point> getCoordinates() const { return coordinates; }

    std::pair<Point, Point> getBbox() const { return bbox; }

    Point getCentroid() const { return centroid; }

    id_type getId() const { return id; }

    void setId(id_type i) { id = i; }

private:
    std::vector<Point> coordinates;
    std::pair<Point, Point> bbox;
    Point centroid;
    id_type id;
};

std::ostream &operator<<(std::ostream &os, const Object &obj)
{
    std::pair<Point, Point> bbox = obj.getBbox();
    os << "[min: " << bbox.first.lat << ", " << bbox.first.lon
       << "max: " << bbox.second.lat << ", " << bbox.second.lon << "]";
    return os;
}

bool inRange(const std::pair<Point, Point> &range, const Point &p)
{
    long double lat = p.lat;
    long double lon = p.lon;

    long double min_lat = range.first.lat;
    long double min_lon = range.first.lon;
    long double max_lat = range.second.lat;
    long double max_lon = range.second.lon;

    if (min_lat > max_lat)
        std::swap(min_lat, max_lat);
    if (min_lon > max_lon)
        std::swap(min_lon, max_lon);
    bool result = ((lat - max_lat) * (lat - min_lat) <= 0) &&
                  ((lon - max_lon) * (lon - min_lon) <= 0);
    return result;
}

enum distance_function
{
    manhattan,
    euclidean
};

long double manhattan_distance(const Point &p1, const Point &p2)
{
    return std::abs(p1.lat - p2.lat) + std::abs(p1.lon - p2.lon);
}

long double euclidean_distance(const Point &p1, const Point &p2)
{
    return std::sqrt(std::pow((p1.lat - p2.lat), 2) +
                     std::pow((p1.lon - p2.lon), 2));
}

#endif
