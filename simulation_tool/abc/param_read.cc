#include "param_read.hh"
ReadData::ReadData()
{
    
}
ReadData::~ReadData()
{

}
std::vector<double>
ReadData::read_single(std::string file_name, int reverse_sing)
{
    std::vector<double> data;
    data.resize(0);
    std::string path_energy_profile = file_name;
    double temp;
    std::ifstream fin;
    fin.open(path_energy_profile.c_str());
    assert(fin);    
    while (fin>>temp) {
            data.push_back(temp);
    }
    if(reverse_sing){
        reverse(data.begin(), data.end());
    }
    fin.close();
    return data;
}
std::vector<double>
ReadData::read_all(std::string file_name, int cloume, int layer_num)
{
    std::vector<double> data;
    data.resize(0);
    int total_colume = layer_num * 4 + 2;
    std::string path_energy_profile = file_name;
    double temp;
    std::ifstream fin;
    fin.open(path_energy_profile.c_str());
    assert(fin);    
    int i = 0;
        while (fin>>temp) {
            if(i % total_colume == cloume){
                // std::cout << temp << std::endl;
                // getchar();
                data.push_back(temp);
            }
            i++;
        }

    fin.close();
    return data;
}