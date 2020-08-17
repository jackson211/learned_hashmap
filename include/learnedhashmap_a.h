#pragma once

#ifndef HASHMAP_A_H
#define HASHMAP_A_H

#include "entry.h"
#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <cstddef>

// LearnedHashMap_A class
template <typename KeyType, typename ValueType, typename ModelType>
class LearnedHashMap_A
{
public:
    LearnedHashMap_A(const LearnedHashMap_A &other);
    const LearnedHashMap_A &operator=(const LearnedHashMap_A &other);

    LearnedHashMap_A(const bool &sort_order,
                     const std::vector<long double> train_x,
                     const std::vector<long double> train_y);

    LearnedHashMap_A(ModelType &model, bool &sort_order,
                     const int MIN_PRED_VALUE, const int MAX_PRED_VALUE);

    virtual ~LearnedHashMap_A();
    int size() const;
    bool isEmpty() const;
    inline int hashFunc(long double value);
    void insertNode(const Point &entry);
    // void insertNode(const std::pair<Point, Point> &entry);
    bool pointSearch(const long double lat, const long double lon,
                     ValueType &value);
    // bool regionSearch(const long double lat, const long double lon,
    //                   std::pair<Point, Point> &value);
    // bool removeNode(const long double lat, const long double lon);
    // bool rangeSearch(long double *min, long double *max,
    //                  std::vector<ValueType> *result);
    // void resize(int newSize);
    // void display_stats(bool showFullStats) const;
    void display() const;

private:
    std::vector<std::vector<ValueType>> table;
    int numSlots;
    int numEntries;
    ModelType _model;
    bool sort_by_lat;
    int MIN_INDEX;
    int MAX_INDEX;

    void createTable(int tableSize)
    {
        if (tableSize == 0)
            tableSize = 1;
        std::vector<ValueType> v;
        table = std::vector<std::vector<ValueType>>(tableSize, v);
        numSlots = tableSize;
        numEntries = 0;
    }

    void deleteTable(std::vector<std::vector<ValueType>> &table)
    {
        // for (int i = 0; i < numSlots; ++i)
        // {
        //     std::vector<ValueType> entry = table[i];
        //     for (auto value : entry)
        //     {
        //         delete value;
        //     }
        //     table[i] = NULL;
        // }
        table.clear();
    }

    // void expandAndRehash()
    // {
    //     HashNode<KeyType, ValueType> **oldTable = table;
    //     int newNumSlots = numSlots * 2 + 1;
    //     createTable(newNumSlots);
    //     for (int i = 0; i < numSlots; i++)
    //     {
    //         for (HashNode<KeyType, ValueType> *node = oldTable[i]; node !=
    //         NULL;
    //              node = node->next)
    //         {
    //             insertNode(node->value);
    //         }
    //     }
    //     deleteTable(oldTable);
    // }
};

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap_A<KeyType, ValueType, ModelType>::LearnedHashMap_A(
    const bool &sort_order, const std::vector<long double> train_x,
    const std::vector<long double> train_y)
    : sort_by_lat(sort_order)
{
    // Train model
    _model.fit(train_x, train_y);
    std::vector<int> pred_result = _model.template predict_list<int>(train_x);
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
    createTable(MAX_INDEX - MIN_INDEX + 1);
}

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap_A<KeyType, ValueType, ModelType>::LearnedHashMap_A(
    ModelType &model, bool &sort_order, const int MIN_PRED_VALUE,
    const int MAX_PRED_VALUE)
    : _model(model), sort_by_lat(sort_order), MIN_INDEX(MIN_PRED_VALUE),
      MAX_INDEX(MAX_PRED_VALUE)
{
    createTable(MAX_INDEX - MIN_INDEX + 1);
}

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap_A<KeyType, ValueType, ModelType>::~LearnedHashMap_A()

{
    deleteTable(table);
}

template <typename KeyType, typename ValueType, typename ModelType>
int LearnedHashMap_A<KeyType, ValueType, ModelType>::size() const
{
    return numEntries;
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap_A<KeyType, ValueType, ModelType>::isEmpty() const
{
    return size() == 0;
}

template <typename KeyType, typename ValueType, typename ModelType>
int LearnedHashMap_A<KeyType, ValueType, ModelType>::hashFunc(long double value)
{
    return _model.template predict<int>(value) - MIN_INDEX;
}

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap_A<KeyType, ValueType, ModelType>::insertNode(
    const Point &entry)
{
    int hashKey = hashFunc(sort_by_lat ? entry.lat : entry.lon);
    std::vector<Point> temp = table[hashKey];

    auto entryLat = entry.lat;
    auto entryLon = entry.lon;
    auto it = temp.begin();
    if (sort_by_lat)
    {
        for (; it != temp.end(); ++it)
        {
            if (entryLon <= it->lon)
                break;
        }
    }
    else
    {
        for (; it != temp.end(); ++it)
        {
            if (entryLat <= it->lat)
                break;
        }
    }
    temp.insert(it, entry);
    table[hashKey] = temp;
    numEntries++;
}

// template <typename KeyType, typename ValueType, typename ModelType>
// void LearnedHashMap_A<KeyType, ValueType, ModelType>::insertNode(
//     const std::pair<Point, Point> &entry)
// {
//     Point p = entry.first;
//     unsigned long hashKey = hashFunc(sort_by_lat ? p.lat : p.lon);
//     HashNode<KeyType, std::pair<Point, Point>> *prev = NULL;
//     HashNode<KeyType, std::pair<Point, Point>> *temp = table[hashKey];

//     while (temp != NULL)
//     {
//         prev = temp;
//         temp = temp->getNext();
//     }

//     if (temp == NULL)
//     {
//         temp = new HashNode<KeyType, std::pair<Point, Point>>(hashKey,
//         entry); if (prev == NULL)
//             // insert as first bucket
//             table[hashKey] = temp;
//         else
//             prev->setNext(temp);
//         numEntries++;
//     }
//     else
//         // just update the value
//         temp->setValue(entry);
// }

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap_A<KeyType, ValueType, ModelType>::pointSearch(
    const long double lat, const long double lon, ValueType &value)
{
    unsigned long hashKey = hashFunc(sort_by_lat ? lat : lon);
    if (hashKey > numSlots)
        return false;
    std::vector<Point> temp = table[hashKey];

    // for (int i = 0; i < temp.size(); i++)
    // {
    //     auto p = temp[i];
    //     if (p.lon == lon)
    //     {
    //         if (p.lat == lat)
    //         {
    //             value = temp[i];
    //             return true;
    //         }
    //     }
    // }

    // Binary search
    int l = 0, r = temp.size();
    if (sort_by_lat)
    {
        while (l <= r)
        {
            int m = l + (r - l) / 2;
            auto temp_lon = temp[m].lon;
            if (temp_lon == lon && temp[m].lat == lat)
            {
                value = temp[m];
                return true;
            }
            if (temp_lon > lon)
                l = m + 1;
            else
                r = m - 1;
        }
    }
    else
    {
        while (l <= r)
        {
            int m = l + (r - l) / 2;
            auto temp_lat = temp[m].lat;
            if (temp_lat == lat && temp[m].lon == lon)
            {
                value = temp[m];
                return true;
            }
            if (temp_lat > lat)
                l = m + 1;
            else
                r = m - 1;
        }
    }

    return false;
}

// template <typename KeyType, typename ValueType, typename ModelType>
// bool LearnedHashMap_A<KeyType, ValueType, ModelType>::regionSearch(
//     const long double lat, const long double lon,
//     std::pair<Point, Point> &value)
// {

//     long double search_value = sort_by_lat ? lat : lon;
//     unsigned long hashKey = hashFunc(search_value);
//     std::cout << "Hashkey: " << hashKey << std::endl;

//     Point search_point{lat, lon};

//     HashNode<KeyType, ValueType> *temp = table[hashKey];
//     HashNode<KeyType, ValueType> *temp_j = table[hashKey];
//     std::pair<Point, Point> temp_result;

//     // If table[hashKey] is not empty, search at current position
//     while (temp != NULL)
//     {
//         temp_result = temp->getValue();
//         if (inRange(temp_result, search_point))
//         {
//             value = temp_result;
//             return true;
//         }
//         temp = temp->getNext();
//     }

//     // Search on both upper and lower bounds
//     int i = hashKey + 1, j = hashKey - 1;
//     while (temp == NULL)
//     {
//         if (i < numSlots)
//         {
//             std::cout << "i: " << i << " ";
//             temp = table[i];
//             while (temp != NULL)
//             {
//                 temp_result = temp->getValue();
//                 if (inRange(temp_result, search_point))
//                 {
//                     value = temp_result;
//                     return true;
//                 }
//                 temp = temp->getNext();
//             }
//             i++;
//         }
//         if (j >= 0)
//         {
//             std::cout << "j: " << j << std::endl;
//             temp = table[j];
//             while (temp != NULL)
//             {
//                 temp_result = temp->getValue();
//                 if (inRange(temp_result, search_point))
//                 {
//                     value = temp_result;
//                     return true;
//                 }
//                 temp = temp->getNext();
//             }
//             j--;
//         }
//         if ((i >= numSlots) && (j < 0))
//             break;
//     }
//     return false;
// }

// template <typename KeyType, typename ValueType, typename ModelType>
// bool LearnedHashMap_A<KeyType, ValueType, ModelType>::removeNode(
//     const long double lat, const long double lon)
// {
//     unsigned long hashKey = hashFunc(sort_by_lat ? lat : lon);
//     HashNode<KeyType, ValueType> *prev = NULL;
//     HashNode<KeyType, ValueType> *temp = table[hashKey];

//     while (temp != NULL)
//     {
//         if (temp->getValue().lon == lon)
//         {
//             if (temp->getValue().lat == lat)
//             {
//                 if (temp->getNext() == NULL)
//                 {
//                     if (prev == NULL)
//                         table[hashKey] = NULL;
//                     else
//                         prev->setNext(NULL);
//                 }
//                 else
//                 {
//                     HashNode<KeyType, ValueType> *next_value =
//                     temp->getNext(); if (prev == NULL)
//                         table[hashKey] = next_value;
//                     else
//                         prev->setNext(next_value);
//                 }
//                 numEntries--;
//                 delete temp;
//                 return true;
//             }
//         }
//         prev = temp;
//         temp = temp->getNext();
//     }
//     return false;
// }

// template <typename KeyType, typename ValueType, typename ModelType>
// bool LearnedHashMap_A<KeyType, ValueType, ModelType>::rangeSearch(
//     long double *min, long double *max, std::vector<ValueType> *result)
// {
//     assert(min[0] <= max[0]);
//     assert(min[1] <= max[1]);

//     long double min_x, min_y, max_y, max_x;

//     // Rearrange x and y range if the data is sort by latitude
//     if (sort_by_lat)
//     {
//         min_x = min[0]; // lat
//         max_x = max[0];
//         min_y = min[1]; // lon
//         max_y = max[1];
//     }
//     else
//     {
//         min_x = min[1]; // lon
//         max_x = max[1];
//         min_y = min[0]; // lat
//         max_y = max[0];
//     }
//     long double x_range = max_x - min_x;
//     long double y_range = max_y - min_y;
//     int min_hashKey = hashFunc(min_x);
//     int max_hashKey = hashFunc(max_x);

//     if (min_hashKey > numSlots)
//         return false;
//     if (min_hashKey < 0)
//         min_hashKey = 0;
//     if (max_hashKey > numSlots)
//         max_hashKey = numSlots - 1;
//     if (min_hashKey > max_hashKey)
//         std::swap(min_hashKey, max_hashKey);

//     std::cout << "\n  x_range: " << x_range << " y_range: " << y_range
//               << " min_HashKey: " << min_hashKey
//               << " max_HashKey: " << max_hashKey << std::endl;

//     for (int i = min_hashKey; i < max_hashKey + 1; i++)
//     {
//         HashNode<KeyType, ValueType> *temp = table[i];
//         while (temp != NULL)
//         {
//             ValueType candidate = temp->getValue();
//             if (((candidate.lon - max_y) * (candidate.lon - min_y) <= 0) &&
//                 ((candidate.lat - max_x) * (candidate.lat - min_x) <= 0))
//                 result->push_back(candidate);
//             temp = temp->getNext();
//         }
//     }

//     if (result->empty())
//         return false;
//     return true;
// }

// template <typename KeyType, typename ValueType, typename ModelType>
// void LearnedHashMap_A<KeyType, ValueType, ModelType>::resize(int newSize)
// {
//     HashNode<KeyType, ValueType> **newTable =
//         new HashNode<KeyType, ValueType> *[newSize];
//     memcpy(newTable, table, numSlots * sizeof(int));

//     numSlots = newSize;
//     delete[] table;
//     table = newTable;
// }

// template <typename KeyType, typename ValueType, typename ModelType>
// void LearnedHashMap_A<KeyType, ValueType, ModelType>::display_stats(
//     bool showFullStats) const
// {
//     std::unordered_map<unsigned long, int> mp;

//     for (int i = 0; i < numSlots; ++i)
//     {
//         HashNode<KeyType, ValueType> *entry = table[i];
//         if (entry == NULL)
//         {
//             mp[i] = 0;
//         }
//         while (entry != NULL)
//         {
//             mp[i]++;
//             entry = entry->getNext();
//         }
//     }
//     int total = 0;
//     int count = 0;

//     if (showFullStats)
//         std::cout << "{ ";
//     for (auto x : mp)
//     {
//         if (x.second != 0)
//         {
//             if (showFullStats)
//             {
//                 std::cout << x.first << ": " << x.second << std::endl;
//             }
//             total += x.second;
//             count++;
//         }
//     }
//     if (showFullStats)
//         std::cout << " }" << std::endl;
//     std::cout << "\n  Total entries: " << total << "\n  Total slots: " <<
//     count
//               << "\n  Average entries per slots: " << total / (double)count
//               << std::endl;
// }

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap_A<KeyType, ValueType, ModelType>::display() const
{
    std::cout << "{" << std::endl;
    for (int i = 0; i < numSlots; ++i)
    {
        std::vector<ValueType> temp = table[i];
        if (!temp.empty())
        {
            for (auto item : temp)
            {
                std::cout << "[(" << item.id << "," << item.lat << ","
                          << item.lon << ")] -> ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "}" << std::endl;
}
#endif
