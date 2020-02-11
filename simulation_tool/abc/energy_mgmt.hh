//////////////////////////////////////////////////////////////////////////////////////
//
// @Creater : Weining Song
// @Date    : 2018-5-3  20:52
// @Brief   : state machine
// @Comment : 
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef ENERGY_MGMT_HH
#define ENERGY_MGMT_HH

#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#define D_WRITE 1
#define D_READ  0

extern unsigned char energy_level;
extern double one_cal_time;
extern double two_cal_time;
extern double total_times;
extern double total_cal_times;
extern double total_trans_times;
extern double total_cal_energy;
extern double total_trans_energy;
extern int break_flag;
extern double current_time;
extern int power_cycle;
extern double energy_rate;
extern double throughput_tile;
extern double every_rate;
extern double power_rate;
extern int m;
extern int n;
extern int g;
class EnergyMGMT : public SimObject
{

public:

    EnergyMGMT();
    ~EnergyMGMT() { }

    int CurSystemState();
    static std::vector<double> energy_harvest_data;
    static std::vector<double> energy_consume_data;

private:

    double havest_latency;
    int leakage_latency;
    double accelerator_latency;

    int leakage_power;

    std::vector<double> readEnergyProfile();
    std::vector<double> readConsumeTrace();


    void EnergyHarvest();
    void Accelerator(double total_energy, double output_bit, int layer);
    void Tile(double times, double delay, double energy, int layer);
    void DoAccelerate();
    void DataTransition(double data_bit, bool trans_type);
    
    void ConsumeEnergy(double energy);
    void LeakageConsume();     
    void startup();
};

#endif    //ENERGY_MGMT_HH