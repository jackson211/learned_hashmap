#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

// #include "HashNode.h"
#include "entry.h"
#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <cstddef>

// Hash node class template
template <typename KeyType, typename ValueType> class HashNode
{
public:
    HashNode();
    HashNode(const KeyType &key, const ValueType &value)
        : _key(key), _value(value), _next(NULL)
    {
    }

    KeyType getKey() const { return _key; }

    ValueType getValue() const { return _value; }

    void setValue(ValueType value) { _value = value; }

    HashNode *getNext() const { return _next; }

    void setNext(HashNode *next) { _next = next; }

private:
    KeyType _key;
    ValueType _value;
    HashNode *_next;
    HashNode(const HashNode &);
    HashNode &operator=(const HashNode &);
};

// LearnedHashMap class
template <typename KeyType, typename ValueType, typename ModelType>
class LearnedHashMap
{
public:
    LearnedHashMap(const LearnedHashMap &other);
    const LearnedHashMap &operator=(const LearnedHashMap &other);

    LearnedHashMap(const bool &sort_order,
                   const std::vector<long double> train_x,
                   const std::vector<long double> train_y)
        : sort_by_lat(sort_order)
    {
        // Train model
        _model.fit(train_x, train_y);
        std::vector<int> pred_result =
            _model.template predict_list<int>(train_x);
        MAX_INDEX = *std::max_element(pred_result.begin(), pred_result.end());
        MIN_INDEX = *std::min_element(pred_result.begin(), pred_result.end());
        std::cout << "  -TRAIN MODEL\n    Training on Linear model\n"
                  << "    Slope: " << _model.getSlope()
                  << ", Intercept: " << _model.getIntercept()
                  << "\n    Prediction:{ Max: " << MAX_INDEX
                  << ", Min: " << MIN_INDEX
                  << ", Max-Min: " << MAX_INDEX - MIN_INDEX + 1 << " }"
                  << std::endl;

        // Initialize table
        Capacity = MAX_INDEX - MIN_INDEX + 1;
        table = new HashNode<KeyType, ValueType> *[Capacity];
        size_t i;
        for (i = 0; i < Capacity; i++)
            table[i] = NULL;
    }

    LearnedHashMap(ModelType &model, bool &sort_order, const int MIN_PRED_VALUE,
                   const int MAX_PRED_VALUE)
        : _model(model), sort_by_lat(sort_order), MIN_INDEX(MIN_PRED_VALUE),
          MAX_INDEX(MAX_PRED_VALUE)
    {
        Capacity = MAX_INDEX - MIN_INDEX + 1;
        table = new HashNode<KeyType, ValueType> *[Capacity];
        for (size_t i = 0; i < Capacity; i++)
            table[i] = NULL;
    }

    ~LearnedHashMap()
    {
        // destroy all buckets one by one
        for (size_t i = 0; i < Capacity; ++i)
        {
            HashNode<KeyType, ValueType> *entry = table[i];
            while (entry != NULL)
            {
                HashNode<KeyType, ValueType> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }
            table[i] = NULL;
        }
        delete[] table;
    }

    size_t getCapacity() const { return Capacity; }

    int hash_function(long double value)
    {
        return _model.template predict<int>(value) - MIN_INDEX;
    }

    void insertNode(const ValueType &entry)
    {
        unsigned long hashKey =
            hash_function(sort_by_lat ? entry.lat : entry.lon);
        HashNode<KeyType, ValueType> *prev = NULL;
        HashNode<KeyType, ValueType> *temp = table[hashKey];

        while (temp != NULL)
        {
            prev = temp;
            temp = temp->getNext();
        }

        if (temp == NULL)
        {
            temp = new HashNode<KeyType, ValueType>(hashKey, entry);
            if (prev == NULL)
                // insert as first bucket
                table[hashKey] = temp;
            else
                prev->setNext(temp);
        }
        else
            // just update the value
            temp->setValue(entry);
    }

    bool pointSearch(const long double lat, const long double lon,
                     ValueType &value)
    {
        unsigned long hashKey = hash_function(sort_by_lat ? lat : lon);
        if (hashKey > Capacity)
            return false;

        HashNode<KeyType, ValueType> *temp = table[hashKey];
        while (temp != NULL)
        {
            if (temp->getValue().lon == lon)
            {
                if (temp->getValue().lat == lat)
                {
                    value = temp->getValue();
                    return true;
                }
            }
            temp = temp->getNext();
        }
        return false;
    }

    bool approximateSearch(const long double lat, const long double lon,
                           ValueType &value)
    {
        // if (pointSearch(lat, lon, value) == true)
        //    return true;

        unsigned long hashKey = hash_function(sort_by_lat ? lat : lon);
        std::cout << "Hashkey: " << hashKey << std::endl;

        HashNode<KeyType, ValueType> *temp = table[hashKey];

        unsigned long i = hashKey;
        while (temp == NULL)
        {
            i++;
            temp = table[i];
            std::cout << "i: " << i << std::endl;
        }
        std::cout << temp->getValue().id << " " << temp->getValue().lat << " "
                  << temp->getValue().lon << std::endl;

        temp = table[hashKey];
        i = hashKey;
        while (temp == NULL)
        {
            i--;
            temp = table[i];
            std::cout << "i: " << i << std::endl;
        }
        std::cout << temp->getValue().id << " " << temp->getValue().lat << " "
                  << temp->getValue().lon << std::endl;
        return true;
    }

    bool removeNode(const long double lat, const long double lon)
    {
        unsigned long hashKey = hash_function(sort_by_lat ? lat : lon);
        HashNode<KeyType, ValueType> *prev = NULL;
        HashNode<KeyType, ValueType> *temp = table[hashKey];

        while (temp != NULL)
        {
            if (temp->getValue().lon == lon)
            {
                if (temp->getValue().lat == lat)
                {
                    if (temp->getNext() == NULL)
                    {
                        if (prev == NULL)
                            table[hashKey] = NULL;
                        else
                            prev->setNext(NULL);
                    }
                    else
                    {
                        HashNode<KeyType, ValueType> *next_value =
                            temp->getNext();
                        if (prev == NULL)
                            table[hashKey] = next_value;
                        else
                            prev->setNext(next_value);
                    }
                    delete temp;
                    return true;
                }
            }
            prev = temp;
            temp = temp->getNext();
        }
        return false;
    }

    bool rangeSearch(long double *min, long double *max,
                     std::vector<ValueType> *result)
    {
        assert(min[0] <= max[0]);
        assert(min[1] <= max[1]);

        long double min_x, min_y, max_y, max_x;

        // Rearrange x and y range if the data is sort by latitude
        if (sort_by_lat)
        {
            min_x = min[0]; // lat
            max_x = max[0];
            min_y = min[1]; // lon
            max_y = max[1];
        }
        else
        {
            min_x = min[1]; // lon
            max_x = max[1];
            min_y = min[0]; // lat
            max_y = max[0];
        }
        long double x_range = max_x - min_x;
        long double y_range = max_y - min_y;
        int min_hashKey = hash_function(min_x);
        int max_hashKey = hash_function(max_x);

        if (min_hashKey > Capacity)
            return false;
        if (min_hashKey < 0)
            min_hashKey = 0;
        if (max_hashKey > Capacity)
            max_hashKey = Capacity - 1;
        if (min_hashKey > max_hashKey)
            std::swap(min_hashKey, max_hashKey);

        std::cout << "\n  x_range: " << x_range << " y_range: " << y_range
                  << " min_HashKey: " << min_hashKey
                  << " max_HashKey: " << max_hashKey << std::endl;

        for (size_t i = min_hashKey; i < max_hashKey + 1; i++)
        {
            HashNode<KeyType, ValueType> *temp = table[i];
            while (temp != NULL)
            {
                ValueType candidate = temp->getValue();
                if (((candidate.lon - max_y) * (candidate.lon - min_y) <= 0) &&
                    ((candidate.lat - max_x) * (candidate.lat - min_x) <= 0))
                    result->push_back(candidate);
                temp = temp->getNext();
            }
        }

        if (result->empty())
            return false;
        return true;
    }

    void resize(size_t newCapacity)
    {
        HashNode<KeyType, ValueType> **newTable =
            new HashNode<KeyType, ValueType> *[newCapacity];
        memcpy(newTable, table, Capacity * sizeof(size_t));

        Capacity = newCapacity;
        delete[] table;
        table = newTable;
    }

    void display_stats(bool showFullStats) const
    {
        std::unordered_map<unsigned long, int> mp;

        for (size_t i = 0; i < Capacity; ++i)
        {
            HashNode<KeyType, ValueType> *entry = table[i];
            if (entry == NULL)
            {
                mp[i] = 0;
            }
            while (entry != NULL)
            {
                mp[i]++;
                entry = entry->getNext();
            }
        }
        int total = 0;
        int count = 0;

        if (showFullStats)
            std::cout << "{ ";
        for (auto x : mp)
        {
            if (x.second != 0)
            {
                if (showFullStats)
                {
                    std::cout << x.first << ": " << x.second << std::endl;
                }
                total += x.second;
                count++;
            }
        }
        if (showFullStats)
            std::cout << " }" << std::endl;
        std::cout << "\n  Total entries: " << total
                  << "\n  Total slots: " << count
                  << "\n  Average entries per slots: " << total / (double)count
                  << std::endl;
    }

    void display() const
    {
        std::cout << "{" << std::endl;
        for (size_t i = 0; i < Capacity; ++i)
        {
            HashNode<KeyType, ValueType> *temp = table[i];
            bool isFirst = true;
            if (temp != NULL)
            {
                while (temp != NULL)
                {
                    if (isFirst)
                    {
                        KeyType key = temp->getKey();
                        std::cout << key << ": ";
                        isFirst = false;
                    }
                    ValueType item = temp->getValue();
                    std::cout << "(" << item.id << "," << item.lat << ","
                              << item.lon << ") -> ";
                    temp = temp->getNext();
                }
                std::cout << std::endl;
            }
        }
        std::cout << "}" << std::endl;
    }

private:
    HashNode<KeyType, ValueType> **table;
    size_t Capacity;
    ModelType _model;
    bool sort_by_lat;
    int MIN_INDEX;
    int MAX_INDEX;
};

#endif
