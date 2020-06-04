#include <algorithm>
#include <string>
#include <assert.h>

#include "HashMap.h"
#include "Linear.h"

struct MyKeyHash
{
    long double w, b;
    unsigned long operator()(const int &k) const
    {
        return w * k + b;
    }
};

int main()
{
    // Read data
    std::string filename = "test.txt";
    ModelData data = utils::read_data(filename, true);

    Linear<long double> model;
    model.fit(data);

    // test set
    std::vector<long double> test_x;
    for (int i = 0; i < data.list.size(); i++)
        test_x.push_back(data.list[i].lat);

    // prediction
    std::vector<int> pred_result = model.predict_list<int>(test_x);
    for (int j = 0; j < data.list.size(); j++)
        std::cout << test_x[j] << " " << pred_result[j] << std::endl;

    // put into hashmap
    long double w = model.getSlope();
    long double b = model.getIntercept();
    MyKeyHash kh = {w, b};
    HashMap<int, DataItem, 10, MyKeyHash> hmap;
    for (int j = 0; j < data.list.size(); j++)
        hmap.put(data.list[j]);

    // retrieve result
    DataItem value;
    hmap.get(value);
    std::cout << value.lat << value.lon << std::endl;
}
