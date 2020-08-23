#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " [input file]\n"
                  << "Input file:\n\ttext file" << std::endl;
        return 1;
    }
    std::string file = argv[1];
    std::cout << "Input file: " << file << std::endl;

    std::vector<uint64_t> data;
    std::ifstream in(file, std::ios::binary);
    // Read size.
    uint64_t size;
    in.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));
    data.resize(size);
    // Read values.
    in.read(reinterpret_cast<char *>(data.data()), size * sizeof(uint64_t));
    in.close();

    for (uint64_t i; i < data.size(); i++)
    {
        std::cout << data[i] << std::endl;
    }

    return 0;
}
