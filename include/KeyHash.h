#pragma once

template <typename K, size_t tableSize>
struct KeyHash
{
    unsigned long operator()(const K &key) const
    {
        return reinterpret_cast<unsigned long>(key) % tableSize;
    }
};

// template <typename K>
// struct KeyHash
// {
//     long double w, b;
//     unsigned long operator()(const K &k) const
//     {
//         return w * k + b;
//     }
// };