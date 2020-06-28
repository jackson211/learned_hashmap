#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

// #include "HashNode.h"
#include "entry.h"
#include <cstddef>
#include <iostream>
#include <unordered_map>

#include <cstddef>

// Hash node class template
template <typename K, typename V>
class HashNode
{
private:
  K _key;
  V _value;
  HashNode *_next;
  HashNode(const HashNode &);
  HashNode &operator=(const HashNode &);

public:
  HashNode();
  HashNode(const K &key, const V &value)
      : _key(key), _value(value), _next(NULL) {}

  K getKey() const { return _key; }

  V getValue() const { return _value; }

  void setValue(V value) { _value = value; }

  HashNode *getNext() const { return _next; }

  void setNext(HashNode *next) { _next = next; }
};

// HashMap class
template <typename K, typename ModelType>
class HashMap
{
public:
  HashMap(ModelType &model, bool &sort_order, const int MIN_PRED_VALUE,
          const int MAX_PRED_VALUE)
      : _model(model),
        sort_by_lat(sort_order),
        MIN(MIN_PRED_VALUE), MAX(MAX_PRED_VALUE)
  {
    Capacity = MAX_PRED_VALUE - MIN_PRED_VALUE + 1;
    table = new HashNode<K, Entry> *[Capacity];
    for (size_t i = 0; i < Capacity; i++)
      table[i] = NULL;
  }

  ~HashMap()
  {
    // destroy all buckets one by one
    for (size_t i = 0; i < Capacity; ++i)
    {
      HashNode<K, Entry> *entry = table[i];

      while (entry != NULL)
      {
        HashNode<K, Entry> *prev = entry;
        entry = entry->getNext();
        delete prev;
      }

      table[i] = NULL;
    }
    delete[] table;
  }

  unsigned long hash_function(long double value)
  {
    return _model.template predict<int>(value) - MIN;
  }

  void insertNode(const Entry &entry)
  {
    unsigned long hashKey = hash_function(sort_by_lat ? entry.lat : entry.lon);
    HashNode<K, Entry> *prev = NULL;
    HashNode<K, Entry> *temp = table[hashKey];

    while (temp != NULL)
    {
      prev = temp;
      temp = temp->getNext();
    }

    if (temp == NULL)
    {
      temp = new HashNode<K, Entry>(hashKey, entry);
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

  bool getNode(const long double lat, const long double lon, Entry &value)
  {
    unsigned long hashKey = hash_function(sort_by_lat ? lat : lon);
    HashNode<K, Entry> *temp = table[hashKey];

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

  bool removeNode(const long double lat, const long double lon)
  {
    unsigned long hashKey = hash_function(sort_by_lat ? lat : lon);
    HashNode<K, Entry> *prev = NULL;
    HashNode<K, Entry> *temp = table[hashKey];

    while (temp != NULL)
    {
      if ((temp->getValue().lon == lon) && (temp->getValue().lat == lat))
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
          HashNode<K, Entry> *next_value = temp->getNext();
          if (prev == NULL)
            table[hashKey] = next_value;
          else
            prev->setNext(next_value);
        }
        delete temp;
        return true;
      }
      prev = temp;
      temp = temp->getNext();
    }
    return false;
  }

  void resize(size_t newCapacity)
  {
    HashNode<K, Entry> **newTable = new HashNode<K, Entry> *[newCapacity];
    memcpy(newTable, table, Capacity * sizeof(size_t));

    Capacity = newCapacity;
    delete[] table;
    table = newTable;
  }

  void display_stats(bool showFullStats)
  {
    std::unordered_map<unsigned long, int> mp;

    for (size_t i = 0; i < Capacity; ++i)
    {
      HashNode<K, Entry> *entry = table[i];
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
    std::cout << " }" << std::endl;
    std::cout << "Total entries: " << total << std::endl;
    std::cout << "Total slots: " << count << std::endl;
    std::cout << "Average: " << total / (double)count << std::endl;
  }

  void display()
  {
    std::cout << "{" << std::endl;
    for (size_t i = 0; i < Capacity; ++i)
    {
      HashNode<K, Entry> *temp = table[i];
      bool isFirst = true;
      if (temp != NULL)
      {
        while (temp != NULL)
        {
          if (isFirst)
          {
            K key = temp->getKey();
            std::cout << key << ": ";
            isFirst = false;
          }
          Entry item = temp->getValue();
          std::cout << "(" << item.id << "," << item.lat << "," << item.lon
                    << ") -> ";
          temp = temp->getNext();
        }
        std::cout << std::endl;
      }
    }
    std::cout << "}" << std::endl;
  }

private:
  HashNode<K, Entry> **table;
  size_t Capacity;
  ModelType _model;
  bool sort_by_lat;
  const int MIN;
  const int MAX;

  HashMap(const HashMap &other);
  const HashMap &operator=(const HashMap &other);
};

#endif
