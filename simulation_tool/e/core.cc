#include "core.hh"

AcceleratorCore::AcceleratorCore()
{
}
AcceleratorCore::AcceleratorCore(double energy, double space)
{
    total_energy = energy;
    power_space  = space * 1000000000000;
}


AcceleratorCore::~AcceleratorCore()
{
}
int 
AcceleratorCore::DataTransaction(double data_bit, int trans_type)
{
    if (trans_type == 1){//write
        total_energy         -= (data_bit / 128) * 0.095;
        total_trans_energy   += (data_bit / 128) * 0.095;
        current_exec_time    += (data_bit / 128) * 20000;
        total_trans_latency  += (data_bit / 128) * 20000;
    } else if (trans_type == 0){//read
        total_energy         -= (data_bit / 128) * 0.037;
        total_trans_energy   += (data_bit / 128) * 0.037;
        current_exec_time    += (data_bit / 128) * 1577;
        total_trans_latency  += (data_bit / 128) * 1577;
    }
    if (current_exec_time >= power_space || total_energy <= 0){
        return 1;
    } else {
        return 0;
    }
}
double 
AcceleratorCore::CalculationRate()
{
    double cal_rate   = 0;
    cal_rate = (total_cal_energy * 1000 / total_cal_latency) / (total_energy * 10000);
    // std::cout << "rate: " << cal_rate << std::endl;
    return cal_rate;
}

double 
AcceleratorCore::CalculationTime(int full_tile_size, double m, double n, double g)
{
    double current_tile_size = m * n * g;
    double calculation_times = full_tile_size / current_tile_size;
    return calculation_times;
}

int 
AcceleratorCore::DoAccelerate(int delay, int full_tile_size, double m, double n, double g, double power, double calculation_times)
{

    total_cal_latency += delay * calculation_times;
    current_exec_time += delay * calculation_times;
    total_energy      -= power * calculation_times * delay / 1000000000;
    total_cal_energy  += power * calculation_times * delay / 1000000000;
    
    if (current_exec_time >= power_space || total_energy <= 0){
        // printf("per times: %d, current_exec_time:%f, total_energy:%f, power: %f, calculation_times: %d, delay:%d, total_cal_energy:%f\n",
        //        delay * calculation_times, current_exec_time, total_energy, power, calculation_times, delay, total_cal_energy);
        return 1;
    } else {
        return 0;
    }
}
int 
AcceleratorCore::DoAccelerate(int delay, int full_tile_size, double m, double n, double g, double power)
{
    double current_tile_size = m * n * g;
    double calculation_times = full_tile_size / current_tile_size;

    total_cal_latency += delay * calculation_times;
    current_exec_time += delay * calculation_times;
    total_energy      -= power * calculation_times * delay / 1000000000;
    total_cal_energy  += power * calculation_times * delay / 1000000000;
    
    if (current_exec_time >= power_space || total_energy <= 0){
        // printf("per times: %f, current_exec_time:%f, total_energy:%f, power: %f, calculation_times: %f, delay:%d, total_cal_energy:%f\n",
        //        delay * calculation_times, current_exec_time, total_energy, power, calculation_times, delay, total_cal_energy);
        return 1;
    } else {
        return 0;
    }
}