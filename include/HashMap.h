#pragma once

#include "HashNode.h"
#include "KeyHash.h"
#include <cstddef>
#include <iostream>

template <typename K, typename V, size_t tableSize, typename F = KeyHash<K, tableSize>>
class HashMap
{
private:
    HashMap(const HashMap &other);
    const HashMap &operator=(const HashMap &other);
    // hash table
    HashNode<K, V> *table[tableSize];
    F hashFunc;

public:
    HashMap() : table(),
                hashFunc()
    {
    }

    ~HashMap()
    {
        // destroy all buckets one by one
        for (size_t i = 0; i < tableSize; ++i)
        {
            HashNode<K, V> *entry = table[i];

            while (entry != NULL)
            {
                HashNode<K, V> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }

            table[i] = NULL;
        }
    }

    bool get(V &value)
    {
        unsigned long hashValue = hashFunc(value.lat);
        std::cout << hashValue << std::endl;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL)
        {
            if (entry->getKey() == hashValue)
            {
                value = entry->getValue();
                return true;
            }

            entry = entry->getNext();
        }

        return false;
    }

    void put(const V &value)
    {
        unsigned long hashKey = hashFunc(value.lat);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashKey];

        while (entry != NULL && entry->getKey() != hashKey)
        {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL)
        {
            entry = new HashNode<K, V>(hashKey, value);

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
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

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
                table[hashValue] = entry->getNext();
            }
            else
            {
                prev->setNext(entry->getNext());
            }

            delete entry;
        }
    }
};
