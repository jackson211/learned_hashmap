#pragma once

#include "HashNode.h"
#include "utils.h"
#include <cstddef>
#include <iostream>

template <typename K, size_t tableSize>
class HashMap
{
private:
    HashMap(const HashMap &other);
    const HashMap &operator=(const HashMap &other);
    // hash table
    HashNode<K, DATAITEM> *table[tableSize];
    long double _w, _b;

public:
    HashMap(long double w, long double b) : table()
    {
        _w = w;
        _b = b;
    }

    ~HashMap()
    {
        // destroy all buckets one by one
        for (size_t i = 0; i < tableSize; ++i)
        {
            HashNode<K, DATAITEM> *entry = table[i];

            while (entry != NULL)
            {
                HashNode<K, DATAITEM> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }

            table[i] = NULL;
        }
    }

    unsigned long hashFunc(K k)
    {
        return _w * k + _b;
    }

    bool get(long double lat, long double lon, DATAITEM &value)
    {
        unsigned long hashKey = hashFunc(lat);
        HashNode<K, DATAITEM> *entry = table[hashKey];

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

    void put(const DATAITEM &value)
    {
        unsigned long hashKey = hashFunc(value.lat);
        HashNode<K, DATAITEM> *prev = NULL;
        HashNode<K, DATAITEM> *entry = table[hashKey];

        while (entry != NULL)
        {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL)
        {
            entry = new HashNode<K, DATAITEM>(hashKey, value);

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
        unsigned long hashKey = hashFunc(key);
        HashNode<K, DATAITEM> *prev = NULL;
        HashNode<K, DATAITEM> *entry = table[hashKey];

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
            HashNode<K, DATAITEM> *entry = table[i];
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
                    DATAITEM item = entry->getValue();
                    std::cout << "(" << item.id << "," << item.lat << "," << item.lon << ") -> ";
                    entry = entry->getNext();
                }
                std::cout << std::endl;
            }
        }
        std::cout << "}" << std::endl;
    }
};
