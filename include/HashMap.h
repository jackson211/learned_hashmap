#pragma once

#ifndef HASHMAP_H
#define HASHMAP_H

#include "HashNode.h"
#include <cstddef>
#include <iostream>

template <typename K, size_t tableSize, typename M> class HashMap {
private:
  HashMap(const HashMap &other);
  const HashMap &operator=(const HashMap &other);
  // hash table
  // HashNode<K, Coord> *table[tableSize];
  std::vector<HashNode<K, Coord>> *table;
  ModelData _data;
  M _model;
  const int _MIN_PRED_VALUE;

public:
  HashMap(ModelData &data, M &model, const int &MIN_PRED_VALUE)
      : table(), _data(data), _model(model), _MIN_PRED_VALUE(MIN_PRED_VALUE) {}

  // ~HashMap()
  // {
  //     // destroy all buckets one by one
  //     for (size_t i = 0; i < tableSize; ++i)
  //     {
  //         HashNode<K, Coord> *entry = table->at(i);

  //         while (entry != NULL)
  //         {
  //             HashNode<K, Coord> *prev = entry;
  //             entry = entry->getNext();
  //             delete prev;
  //         }

  //         table[i] = NULL;
  //     }
  // }

  unsigned long hash_key(long double value) {
    return _model.predict(value) - _MIN_PRED_VALUE;
  }

  HashNode<K, Coord> *get_node(const long double &value) {
    return &table->at(hash_key(value));
  }

  bool get(const long double lat, const long double lon, Coord &value) {
    // unsigned long hashKey = hash_key(lat);
    HashNode<K, Coord> *entry = get_node(lat);

    while (entry != NULL) {
      if (entry->getValue().lon == lon) {
        if (entry->getValue().lat == lat) {
          value = entry->getValue();
          return true;
        }
      }
      entry = entry->getNext();
    }

    return false;
  }

  void put(const Coord &value) {
    unsigned long hashKey = hash_key(value.lat);
    HashNode<K, Coord> *prev = NULL;
    // HashNode<K, Coord> *entry = table[hashKey];

    HashNode<K, Coord> *entry = get_node(value.lat);

    while (entry != NULL) {
      prev = entry;
      entry = entry->getNext();
    }

    if (entry == NULL) {
      entry = new HashNode<K, Coord>(hashKey, value);

      if (prev == NULL) {
        // insert as first bucket
        table->at(hashKey) = *entry;
      } else {
        prev->setNext(entry);
      }
    } else {
      // just update the value
      entry->setValue(value);
    }
  }

  void test(std::vector<long double> &test_set) {
    int count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int k = 0; k < test_set.size(); k += 2) {
      Coord testValue;
      get(test_set[k], test_set[k + 1], testValue);
      assert(testValue.isEqual(_data.list[k / 2]));
      count++;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << "Total look up time : " << duration.count() << " nanoseconds"
              << std::endl;
    std::cout << "Average look up time : "
              << duration.count() / (test_set.size() / 2) << " nanoseconds"
              << std::endl;
    std::cout << "Correct item found: " << count << std::endl;
  }

  void remove(const K &key) {
    unsigned long hashKey = hash_key(key);
    HashNode<K, Coord> *prev = NULL;
    HashNode<K, Coord> *entry = table[hashKey];

    while (entry != NULL && entry->getKey() != key) {
      prev = entry;
      entry = entry->getNext();
    }

    if (entry == NULL) {
      // key not found
      return;
    } else {
      if (prev == NULL) {
        // remove first bucket of the list
        table[hashKey] = entry->getNext();
      } else {
        prev->setNext(entry->getNext());
      }

      delete entry;
    }
  }

  void printHashMap() {
    std::cout << "{" << std::endl;
    for (size_t i = 0; i < tableSize; ++i) {
      HashNode<K, Coord> *entry = table[i];
      bool isFirst = true;
      if (entry != NULL) {
        while (entry != NULL) {
          if (isFirst) {
            K key = entry->getKey();
            std::cout << key << ": ";
            isFirst = false;
          }
          Coord item = entry->getValue();
          std::cout << "(" << item.id << "," << item.lat << "," << item.lon
                    << ") -> ";
          entry = entry->getNext();
        }
        std::cout << std::endl;
      }
    }
    std::cout << "}" << std::endl;
  }
};

#endif
