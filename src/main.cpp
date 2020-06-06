#include <algorithm>
#include <string>
#include <assert.h>

#include "HashMap.h"
#include "KeyHash.h"
#include "Linear.h"
#include "utils.h"

// struct LinearKeyHash
// {
//     long double w, b;
//     unsigned long operator()(const int &k) const
//     {
//         return w * k + b;
//     }
// };

int main()
{
    // Read data
    std::string filename = "test.txt";
    ModelData data = utils::read_data(filename, true);
    data.print();

    std::vector<long double> train_lat;
    std::vector<long double> train_lon;
    std::vector<long double> train_y;
    for (int i; i < data.size(); i++)
    {
        train_lat.push_back(data.list[i].lat);
        train_lon.push_back(data.list[i].lon);
        train_y.push_back(i);
    }

    // Train
    Linear<long double> model_lat;
    if (data.sortByLat)
    {
        model_lat.fit(train_lat, train_y);
    }

    // prediction
    std::vector<int> pred_result = model_lat.predict_list<int>(train_lat);
    for (int j = 0; j < data.list.size(); j++)
        std::cout << train_lat[j] << " " << pred_result[j] << std::endl;

    // put into hashmap
    HashMap<int, 10> hmap(model_lat.getSlope(), model_lat.getIntercept());
    for (int j = 0; j < data.list.size(); j++)
    {
        std::cout << data.list[j].lat << " " << data.list[j].lon << std::endl;
        hmap.put(data.list[j]);
    }

    hmap.printHashMap();

    // retrieve result
    // DATAITEM value = data.list[0];
    // hmap.get(value);
    // std::cout << value.lat << value.lon << std::endl;
}
