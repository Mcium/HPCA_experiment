#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <string>

class ReadData
{
    public:
        ReadData();
        ~ReadData();

        std::vector<double> read_single(std::string file_name, int reverse_sing);
        std::vector<double> read_all(std::string file_name, int cloume, int layer_num);
};
