#pragma once

#include "HashNode.h"
#include "KeyHash.h"
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
    // F hashFunc;

    long double _w, _b;

public:
    HashMap(long double w, long double b) : table()
    // hashFunc()
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

    unsigned long hashFunc(int k)
    {
        return _w * k + _b;
    }

    bool get(DATAITEM &value)
    {
        unsigned long hashKey = hashFunc(value.lat);
        std::cout << hashKey << std::endl;
        HashNode<K, DATAITEM> *entry = table[hashKey];

        while (entry != NULL)
        {
            if (entry->getKey() == hashKey)
            {
                value = entry->getValue();
                return true;
            }

            entry = entry->getNext();
        }

        return false;
    }

    void put(const DATAITEM &value)
    {
        unsigned long hashKey = hashFunc(value.lat);
        std::cout << hashKey << std::endl;
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
        std::cout << "Hash Map" << std::endl;
        for (size_t i = 0; i < tableSize; ++i)
        {
            HashNode<K, DATAITEM> *entry = table[i];
            while (entry != NULL)
            {
                DATAITEM item = entry->getValue();
                K key = entry->getKey();
                std::cout << "Key: " << key << std::endl;
                std::cout << "Item: " << item.lat << item.lon << std::endl;
                entry = entry->getNext();
            }
        }
    }
};
