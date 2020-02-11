#include <iostream>
#include <math.h>
class AcceleratorCore
{
    private:
        /* data */
        double total_energy;

        double cal_energy          = 0;
        double trans_energy        = 0;
        double cal_latency         = 0;
        double trans_latency       = 0;
         
        double current_exec_time   = 0;
        double total_trans_energy  = 0;
        double total_trans_latency = 0;

        double total_cal_latency   = 0;
        double total_cal_energy    = 0;

        double power_space;

        
    public:
        AcceleratorCore();
        AcceleratorCore(double energy, double space);
        ~AcceleratorCore();

        int DataTransaction(double data_bit, int trans_type);

        int DoAccelerate(int delay, int full_tile_size, double m, double n, double g, double power);
        double CalculationRate();

};
