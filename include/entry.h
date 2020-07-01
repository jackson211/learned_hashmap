#pragma once

#ifndef ENTRY_H
#define ENTRY_H

#include <cstdint>

typedef uint64_t id_type;

struct Entry
{
    long double lat, lon;
    id_type id;

    void setId(id_type i) { id = i; }

    bool operator==(const Entry &t) const
    {
        return (this->lat == t.lat && this->lon == t.lon);
    }
};

#endif
