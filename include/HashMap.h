#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

#include "HashNode.h"
// #include "KeyHash.h"
#include "utils.h"
#include <cstddef>
#include <iostream>

template <typename K, size_t tableSize, typename F>
class HashMap
{
private:
    HashMap(const HashMap &other);
    const HashMap &operator=(const HashMap &other);
    // hash table
    HashNode<K, Coord> *table[tableSize];
    long double _w, _b;
    F hashFunc;

public:
    HashMap(long double w, long double b) : table(), _w(w), _b(b), hashFunc()
    {
    }

    ~HashMap()
    {
        // destroy all buckets one by one
        for (size_t i = 0; i < tableSize; ++i)
        {
            HashNode<K, Coord> *entry = table[i];

            while (entry != NULL)
            {
                HashNode<K, Coord> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }

            table[i] = NULL;
        }
    }

    bool get(long double lat, long double lon, Coord &value)
    {
        unsigned long hashKey = hashFunc(lat, _w, _b);
        HashNode<K, Coord> *entry = table[hashKey];

        while (entry != NULL)
        {
            if (entry->getValue().lon == lon)
            {
                if (entry->getValue().lat == lat)
                {
                    value = entry->getValue();
                    return true;
                }
            }
            entry = entry->getNext();
        }

        return false;
    }

    void put(const Coord &value)
    {
        unsigned long hashKey = hashFunc(value.lat, _w, _b);
        HashNode<K, Coord> *prev = NULL;
        HashNode<K, Coord> *entry = table[hashKey];

        while (entry != NULL)
        {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL)
        {
            entry = new HashNode<K, Coord>(hashKey, value);

            if (prev == NULL)
            {
                // insert as first bucket
                table[hashKey] = entry;
            }
            else
            {
                prev->setNext(entry);
            }
        }
        else
        {
            // just update the value
            entry->setValue(value);
        }
    }

    void remove(const K &key)
    {
        unsigned long hashKey = hashFunc(key, _w, _b);
        HashNode<K, Coord> *prev = NULL;
        HashNode<K, Coord> *entry = table[hashKey];

        while (entry != NULL && entry->getKey() != key)
        {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL)
        {
            // key not found
            return;
        }
        else
        {
            if (prev == NULL)
            {
                // remove first bucket of the list
                table[hashKey] = entry->getNext();
            }
            else
            {
                prev->setNext(entry->getNext());
            }

            delete entry;
        }
    }

    void printHashMap()
    {
        std::cout << "{" << std::endl;
        for (size_t i = 0; i < tableSize; ++i)
        {
            HashNode<K, Coord> *entry = table[i];
            bool isFirst = true;
            if (entry != NULL)
            {
                while (entry != NULL)
                {
                    if (isFirst)
                    {
                        K key = entry->getKey();
                        std::cout << key << ": ";
                        isFirst = false;
                    }
                    Coord item = entry->getValue();
                    std::cout << "(" << item.id << "," << item.lat << "," << item.lon << ") -> ";
                    entry = entry->getNext();
                }
                std::cout << std::endl;
            }
        }
        std::cout << "}" << std::endl;
    }
};

#endif