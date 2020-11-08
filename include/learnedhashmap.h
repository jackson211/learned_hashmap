#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

#include "entry.h"
#include <cstddef>
#include <iostream>
#include <limits>
#include <queue>
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
                   const std::vector<long double> train_y);

    LearnedHashMap(ModelType &model, bool &sort_order, const int MIN_PRED_VALUE,
                   const int MAX_PRED_VALUE);

    virtual ~LearnedHashMap();
    int size() const;
    bool isEmpty() const;
    inline int hashFunc(long double value);
    void insertNode(const Point &entry);
    void insertNode(const std::pair<Point, Point> &entry);
    bool removeNode(const long double lat, const long double lon);

    // Search algorithms
    bool pointSearch(const long double lat, const long double lon,
                     ValueType &result);
    bool rangeSearch(long double *min, long double *max,
                     std::vector<ValueType> *result);
    void nearestNeighborSearch(const Point &p, std::vector<ValueType> &result,
                               distance_function df);
    bool regionSearch(const long double lat, const long double lon,
                      std::pair<Point, Point> &value);
    void resize(int newSize);
    void display_stats(bool showFullStats) const;
    void display() const;

private:
    HashNode<KeyType, ValueType> **table;
    int numSlots;
    int numEntries;
    int numHeadNode;
    ModelType _model;
    bool sort_by_lat;
    int MIN_INDEX;
    int MAX_INDEX;

    void createTable(int tableSize)
    {
        if (tableSize == 0)
            tableSize = 1;
        table = new HashNode<KeyType, ValueType> *[tableSize];
        for (int i = 0; i < tableSize; i++)
            table[i] = NULL;
        numSlots = tableSize;
        numEntries = 0;
        numHeadNode = 0;
    }

    void deleteTable(HashNode<KeyType, ValueType> **table)
    {
        for (int i = 0; i < numSlots; ++i)
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

    void expandAndRehash()
    {
        HashNode<KeyType, ValueType> **oldTable = table;
        int newNumSlots = numSlots * 2 + 1;
        createTable(newNumSlots);
        for (int i = 0; i < numSlots; i++)
        {
            for (HashNode<KeyType, ValueType> *node = oldTable[i]; node != NULL;
                 node = node->next)
            {
                insertNode(node->value);
            }
        }
        deleteTable(oldTable);
    }
};

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap<KeyType, ValueType, ModelType>::LearnedHashMap(
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
LearnedHashMap<KeyType, ValueType, ModelType>::LearnedHashMap(
    ModelType &model, bool &sort_order, const int MIN_PRED_VALUE,
    const int MAX_PRED_VALUE)
    : _model(model), sort_by_lat(sort_order), MIN_INDEX(MIN_PRED_VALUE),
      MAX_INDEX(MAX_PRED_VALUE)
{
    createTable(MAX_INDEX - MIN_INDEX + 1);
}

template <typename KeyType, typename ValueType, typename ModelType>
LearnedHashMap<KeyType, ValueType, ModelType>::~LearnedHashMap()

{
    deleteTable(table);
}

template <typename KeyType, typename ValueType, typename ModelType>
int LearnedHashMap<KeyType, ValueType, ModelType>::size() const
{
    return numEntries;
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap<KeyType, ValueType, ModelType>::isEmpty() const
{
    return size() == 0;
}

template <typename KeyType, typename ValueType, typename ModelType>
int LearnedHashMap<KeyType, ValueType, ModelType>::hashFunc(long double value)
{
    return _model.template predict<int>(value) - MIN_INDEX;
}

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::resize(int newSize)
{
    HashNode<KeyType, ValueType> **newTable =
        new HashNode<KeyType, ValueType> *[newSize];
    memcpy(newTable, table, numSlots * sizeof(int));

    numSlots = newSize;
    delete[] table;
    table = newTable;
}
template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::insertNode(
    const Point &entry)
{
    unsigned long hashKey = hashFunc(sort_by_lat ? entry.lat : entry.lon);
    HashNode<KeyType, Point> *prev = NULL;
    HashNode<KeyType, Point> *temp = table[hashKey];
    bool isHead = false;

    if (temp != NULL)
        isHead = true;

    while (temp != NULL)
    {
        prev = temp;
        temp = temp->getNext();
    }

    if (temp == NULL)
    {
        temp = new HashNode<KeyType, Point>(hashKey, entry);
        if (prev == NULL)
            // insert as first bucket
            table[hashKey] = temp;
        else
            prev->setNext(temp);
        numEntries++;
    }
    else
        // just update the value
        temp->setValue(entry);

    if (isHead)
        numHeadNode++;
}

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::insertNode(
    const std::pair<Point, Point> &entry)
{
    Point p = entry.first;
    unsigned long hashKey = hashFunc(sort_by_lat ? p.lat : p.lon);
    HashNode<KeyType, std::pair<Point, Point>> *prev = NULL;
    HashNode<KeyType, std::pair<Point, Point>> *temp = table[hashKey];

    while (temp != NULL)
    {
        prev = temp;
        temp = temp->getNext();
    }

    if (temp == NULL)
    {
        temp = new HashNode<KeyType, std::pair<Point, Point>>(hashKey, entry);
        if (prev == NULL)
            // insert as first bucket
            table[hashKey] = temp;
        else
            prev->setNext(temp);
        numEntries++;
    }
    else
        // just update the value
        temp->setValue(entry);
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap<KeyType, ValueType, ModelType>::removeNode(
    const long double lat, const long double lon)
{
    unsigned long hashKey = hashFunc(sort_by_lat ? lat : lon);
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
                    HashNode<KeyType, ValueType> *next_value = temp->getNext();
                    if (prev == NULL)
                        table[hashKey] = next_value;
                    else
                        prev->setNext(next_value);
                }
                numEntries--;
                delete temp;
                return true;
            }
        }
        prev = temp;
        temp = temp->getNext();
    }
    return false;
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap<KeyType, ValueType, ModelType>::pointSearch(
    const long double lat, const long double lon, ValueType &result)
{
    unsigned long hashKey = hashFunc(sort_by_lat ? lat : lon);
    if (hashKey > numSlots)
        return false;

    HashNode<KeyType, ValueType> *temp = table[hashKey];
    while (temp != NULL)
    {
        if (temp->getValue().lon == lon)
        {
            if (temp->getValue().lat == lat)
            {
                result = temp->getValue();
                return true;
            }
        }
        temp = temp->getNext();
    }
    return false;
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap<KeyType, ValueType, ModelType>::rangeSearch(
    long double *min, long double *max, std::vector<ValueType> *result)
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
    int min_hashKey = hashFunc(min_x);
    int max_hashKey = hashFunc(max_x);

    if (min_hashKey > numSlots)
        return false;
    if (min_hashKey < 0)
        min_hashKey = 0;
    if (max_hashKey > numSlots)
        max_hashKey = numSlots - 1;
    if (min_hashKey > max_hashKey)
        std::swap(min_hashKey, max_hashKey);

    std::cout << "\n  x_range: " << x_range << " y_range: " << y_range
              << " min_HashKey: " << min_hashKey
              << " max_HashKey: " << max_hashKey << std::endl;

    for (int i = min_hashKey; i < max_hashKey + 1; i++)
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

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::nearestNeighborSearch(
    const Point &p, std::vector<ValueType> &result, distance_function df)
{
    unsigned long hashKey = hashFunc(sort_by_lat ? p.lat : p.lon);
    if (hashKey > numSlots)
        return;

    long double (*calculate_distance)(const Point &, const Point &);
    switch (df)
    {
    case manhattan:
        calculate_distance = &manhattan_distance;
    case euclidean:
        calculate_distance = &euclidean_distance;
    }
    // Init Priority Queue
    auto comp = [](const std::pair<long double, ValueType> &a,
                   const std::pair<long double, ValueType> &b) {
        return a.first < b.first;
    };

    std::priority_queue<std::pair<long double, ValueType>,
                        std::vector<std::pair<long double, ValueType>>,
                        decltype(comp)>
        q(comp);

    int k = 2;
    long double previous_local_max = 0;
    long double local_min = std::numeric_limits<float>::infinity();

    // Search locally
    HashNode<KeyType, ValueType> *temp = table[hashKey];
    while (temp != NULL)
    {
        ValueType temp_p = temp->getValue();
        if (p.lat != temp_p.lat && p.lon != temp_p.lon)
        {
            long double p_distance = calculate_distance(temp_p, p);
            if (p_distance < local_min)
            {
                local_min = p_distance;
                // result.push_back(temp_p);
                std::pair<long double, ValueType> value_pair =
                    std::make_pair(p_distance, temp_p);
                q.push(value_pair);
            }
            if (previous_local_max < p_distance)
            {
                previous_local_max = p_distance;
            }
        }
        temp = temp->getNext();
    }
    // std::cout << previous_local_max << " " << local_min << std::endl;

    // Expansion Search
    unsigned long i = hashKey + 1;
    unsigned long j = hashKey - 1;
    // std::cout << "numSlots: " << numSlots << std::endl;
    while ((local_min <= previous_local_max) && (i < numSlots) && (j > 0))
    {
        HashNode<KeyType, ValueType> *temp = table[i];
        while (temp != NULL)
        {
            ValueType temp_p = temp->getValue();
            if (p.lat != temp_p.lat && p.lon != temp_p.lon)
            {
                long double p_distance = calculate_distance(temp_p, p);
                if (p_distance < local_min)
                {
                    local_min = p_distance;
                    // result.push_back(temp_p);
                    std::pair<long double, ValueType> value_pair =
                        std::make_pair(p_distance, temp_p);
                    q.push(value_pair);
                    // std::cout << "Push" << value_pair.first << ", "
                    //           << temp_p.lat << " - " << temp_p.lon <<
                    //           std::endl;
                }
                if (previous_local_max < p_distance)
                {
                    previous_local_max = p_distance;
                }
            }
            temp = temp->getNext();
        }

        HashNode<KeyType, ValueType> *temp_b = table[j];
        while (temp_b != NULL)
        {
            ValueType temp_p = temp_b->getValue();
            if (p.lat != temp_p.lat && p.lon != temp_p.lon)
            {
                long double p_distance = calculate_distance(temp_p, p);
                if (p_distance < local_min)
                {
                    local_min = p_distance;
                    // result.push_back(temp_p);
                    std::pair<long double, ValueType> value_pair =
                        std::make_pair(p_distance, temp_p);
                    q.push(value_pair);
                    // std::cout << "Push" << value_pair.first << ", "
                    //           << temp_p.lat << " - " << temp_p.lon <<
                    //           std::endl;
                }
                if (previous_local_max < p_distance)
                {
                    previous_local_max = p_distance;
                }
            }
            temp_b = temp_b->getNext();
        }
        i++;
        j--;
    }

    // std::cout << "Print queue" << std::endl;
    // while (!q.empty())
    // {
    //     std::cout << q.top().first << " ";
    //     q.pop();
    // }
    // std::cout << '\n';
}

template <typename KeyType, typename ValueType, typename ModelType>
bool LearnedHashMap<KeyType, ValueType, ModelType>::regionSearch(
    const long double lat, const long double lon,
    std::pair<Point, Point> &value)
{

    long double search_value = sort_by_lat ? lat : lon;
    unsigned long hashKey = hashFunc(search_value);
    std::cout << "Hashkey: " << hashKey << std::endl;

    Point search_point{lat, lon};

    HashNode<KeyType, ValueType> *temp = table[hashKey];
    std::pair<Point, Point> temp_result;

    // If table[hashKey] is not empty, search at current position
    while (temp != NULL)
    {
        temp_result = temp->getValue();
        if (inRange(temp_result, search_point))
        {
            value = temp_result;
            return true;
        }
        temp = temp->getNext();
    }

    // Search on both upper and lower bounds
    int i = hashKey + 1, j = hashKey - 1;
    while (temp == NULL)
    {
        if (i < numSlots)
        {
            std::cout << "i: " << i << " ";
            temp = table[i];
            while (temp != NULL)
            {
                temp_result = temp->getValue();
                if (inRange(temp_result, search_point))
                {
                    value = temp_result;
                    return true;
                }
                temp = temp->getNext();
            }
            i++;
        }
        if (j >= 0)
        {
            std::cout << "j: " << j << std::endl;
            temp = table[j];
            while (temp != NULL)
            {
                temp_result = temp->getValue();
                if (inRange(temp_result, search_point))
                {
                    value = temp_result;
                    return true;
                }
                temp = temp->getNext();
            }
            j--;
        }
        if ((i >= numSlots) && (j < 0))
            break;
    }
    return false;
}

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::display_stats(
    bool showFullStats) const
{
    // std::unordered_map<unsigned long, int> mp;

    // for (int i = 0; i < numSlots; ++i)
    // {
    //     HashNode<KeyType, ValueType> *entry = table[i];
    //     if (entry == NULL)
    //     {
    //         mp[i] = 0;
    //     }
    //     while (entry != NULL)
    //     {
    //         mp[i]++;
    //         entry = entry->getNext();
    //     }
    // }
    // int total = 0;
    // int count = 0;

    // if (showFullStats)
    //     std::cout << "{ ";
    // for (auto x : mp)
    // {
    //     if (x.second != 0)
    //     {
    //         if (showFullStats)
    //         {
    //             std::cout << x.first << ": " << x.second << std::endl;
    //         }
    //         total += x.second;
    //         count++;
    //     }
    // }
    // if (showFullStats)
    //     std::cout << " }" << std::endl;
    std::cout << "\n  Total entries: " << numEntries
              << "\n  Total head nodes: " << numHeadNode
              << "\n  Total slots: " << numSlots
              << "\n  Loading factor: " << numHeadNode / (double)numSlots
              << std::endl;
}

template <typename KeyType, typename ValueType, typename ModelType>
void LearnedHashMap<KeyType, ValueType, ModelType>::display() const
{
    std::cout << "{" << std::endl;
    for (int i = 0; i < numSlots; ++i)
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
                // if (typeid(item) == typeid(std::pair<Point, Point>))
                // {
                //     std::cout << "[(" << item.first.id << "," <<
                //     item.first.lat
                //               << "," << item.first.lon << item.second.id
                //               <<
                //               ","
                //               << item.second.lat << "," <<
                //               item.second.lon
                //               << ")] -> ";
                // }
                // else
                // {
                std::cout << "[(" << item.id << "," << item.lat << ","
                          << item.lon << ")] -> ";
                // }
                temp = temp->getNext();
            }
            std::cout << std::endl;
        }
    }
    std::cout << "}" << std::endl;
}
#endif
