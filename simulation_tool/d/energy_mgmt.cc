//////////////////////////////////////////////////////////////////////////////////////
//
// @Creater : Weining Song
// @Date    : 2018-5-3  20:52
// @Brief   : state state machine
// @Comment : 
//
//////////////////////////////////////////////////////////////////////////////////////
#include "energy_mgmt.hh"

std::vector<double> EnergyMGMT::energy_harvest_data;
std::vector<double> EnergyMGMT::energy_consume_data;
   
unsigned char energy_level       = 0;
double        one_cal_time       = 1;
double        two_cal_time       = 1;
double        total_times        = 0;
double        total_cal_times    = 0;
double        total_cal_energy   = 0;
int           break_flag         = 0;
double        current_time       = 0;
int           power_cycle        = 0;
double        total_trans_times  = 0;  
double        total_trans_energy = 0;
double        energy_rate        = 0;
double        every_rate         = 0;
double        throughput_tile    = 0;
double        power_rate         = 0;
    int                     m = 0;
    int                     n = 0;
    int                     g = 0;

EnergyMGMT::EnergyMGMT(EnergyMGMTParams *params) : SimObject(params), havest_latency(10),
    leakage_latency(1000000000), accelerator_latency(10), leakage_power(1), 
    event_leakage_consume(*this), event_energy_harvest(*this), event_doaccelerate(*this)
    {
        energy_harvest_data.resize(0);
    }

void 
EnergyMGMT::startup()
{
    ENERGY_LIMITATION = 5500000000000;
    EnergyObject::Total_energy = 0;
    energy_harvest_data = readEnergyProfile();
    LeakageConsume();
    EnergyHarvest();
    //DoAccelerate();
}

void 
EnergyMGMT::LeakageConsume()
{
    EnergyObject::Total_energy -= leakage_power;
    DPRINTF(EnergyConsume, "leakage consume: %d. Total energy remain: %d.\n", 
        leakage_power, EnergyObject::Total_energy);
    total_consume += leakage_power;
    total_leakage_consume += leakage_power;
    if (EnergyObject::Total_energy <= 0){
        EnergyObject::Total_energy = 0;
        total_consume -= leakage_power;
        total_leakage_consume -= leakage_power;
    }
    schedule(event_leakage_consume, curTick() + leakage_latency);
}
void 
EnergyMGMT::DataTransition(double data_bit, bool trans_type)
{
    if (trans_type == 1){//write
        EnergyObject::Total_energy -= data_bit / 256 * 0.194;
        total_trans_energy         += data_bit / 256 * 0.194;
        current_time               += data_bit / 256 * 20000;
        total_trans_times          += data_bit / 256 * 20000;
    } else if (trans_type == 0){//read
        EnergyObject::Total_energy -= data_bit / 256 * 0.109;
        //printf("tans energy: %f\n", data_bit / 128 * 0.093);
        total_trans_energy         += data_bit / 256 * 0.109;
        current_time               += data_bit / 256 * 1123;
        total_trans_times          += data_bit / 256 * 1123;
    }
    
}

void 
EnergyMGMT::DoAccelerate()
{
    double layer_input_one_bit  = 5*5*6; //lenet 1024
    double layer_input_two_bit  = 5*5*6*16; //lenet 1024
    double layer_input_three_bit = 1600; //lenet 1600
    int one_c_times = 28 * 28;
    int two_c_times = 10 * 10;
    int layer = 0;
    double total_thr_energy = EnergyObject::Total_energy;
    double throughput_one = 0;
    double throughput_two = 0;
    int m_one = 0;
    int n_one = 0;
    int g_one = 0;
    int m_two = 0;
    int n_two = 0;
    int g_two = 0;
    static double current_work_times = 0;
    if (EnergyObject::Total_energy != 0){

        /* layer one */
        if (energy_level != 0){
            
            while (1){
                if (break_flag == 1){
                    break_flag = 0;
                    EnergyObject::Total_energy = 0;
                    break;
                }
                if (one_cal_time <= one_c_times){
                layer = 1;
                Accelerator(total_thr_energy, layer_input_three_bit, layer);
                throughput_one = throughput_tile;
                m_one = m;
                n_one = n;
                g_one = g;
                } else if (two_cal_time <= two_c_times){
                    layer = 2;
                    Accelerator(total_thr_energy, layer_input_three_bit, layer);
                    throughput_two = throughput_tile;
                    m_two = m;
                    n_two = n;
                    g_two = g;
                    if (two_cal_time >= two_c_times){
                        DataTransition(layer_input_one_bit + layer_input_two_bit, D_READ);
                        //DataTransition(layer_input_three_bit, D_WRITE);
                        if (current_time >= 100000000000){
                            total_trans_times -= current_time - 100000000000;
                            total_times--;
                            current_work_times--;
                            one_cal_time = 0;
                            two_cal_time = 0;
                            break_flag = 0;
                            current_time = 0;
                            EnergyObject::Total_energy = 0;
                            break;
                        } 
                        if (EnergyObject::Total_energy <= 0){
                            total_times--;
                            current_work_times--;
                            one_cal_time = 0;
                            two_cal_time = 0;
                            total_trans_energy -= EnergyObject::Total_energy;
                            EnergyObject::Total_energy = 0;
                            break_flag = 0;
                            break;
                        }
                        current_time = 0;
                        one_cal_time = 0;
                        two_cal_time = 0;
                        total_times++;
                        current_work_times++;
                        layer = 1;
                        // printf("total network times: %f, current energy is: %f total trans energy: %f total cal energy: %f\n"
                        // "total time: %f\n",
                        // total_times, EnergyObject::Total_energy, total_trans_energy, total_cal_energy, total_cal_times);
                        //  getchar();
                    }
                }
            }
        }
        static double energy_rate_trans = 0;
        energy_rate_trans += (total_trans_energy) / total_thr_energy;
        energy_rate += (total_cal_energy) / total_thr_energy;
        
        // printf("%f%f%f",total_cal_energy, total_trans_energy, total_thr_energy );

        static double total_thr = 0;
        static double every_thr = 0;
        static double every_power = 0;
        
        
        //power_cycle++;
        total_thr += throughput_tile;
        every_power += power_rate;
        every_thr  += total_thr / power_cycle;//++ bimian unuse
        every_rate += energy_rate / power_cycle;//++ bimian unuse
        // printf("finish one  power cycle! total network times: %f, "
        //        "total power cycle: %d, "
        //        //"total trans power: %f, "
        //        //"total trans time:  %f, "
        //        "throughput: %f, "
        //        "single cal rate: %f, energy rate: %f, power rate: %f, trans rate: %f\n", total_times / power_cycle, power_cycle, 
        //        every_thr, (total_cal_energy) / total_thr_energy, every_rate, every_power / power_cycle, energy_rate_trans / power_cycle);
        printf("%d,%f,%f,%f,%f,%f,%d,%d*%d*%d,%d*%d*%d\n", power_cycle, current_work_times, (throughput_one + throughput_two) / 2, 
               total_cal_energy / total_thr_energy, 
               total_trans_energy / total_thr_energy,
               power_rate, energy_level, m_one, n_one, g_one, m_two, n_two, g_two);
        current_work_times = 0;
        total_cal_energy = 0;
        total_trans_energy = 0;
        throughput_tile = 0;
        power_rate = 0;
        m_one = 0;
        n_one = 0;
        g_one = 0;
        m_two = 0;
        n_two = 0;
        g_two = 0;
        throughput_one = 0;
        throughput_two = 0;
        EnergyObject::Total_energy = 0;
        // getchar();
        //schedule(event_doaccelerate, curTick() + accelerator_latency);
        schedule(event_energy_harvest, curTick() + havest_latency);
    } else {
        //power_cycle++;
        EnergyObject::Total_energy = 0;
        printf("%d,%f,%f,%f,%f,%f,%d,%d*%d*%d,%d*%d*%d\n", power_cycle, current_work_times, (throughput_one + throughput_two) / 2, 
               total_cal_energy / total_thr_energy, 
               total_trans_energy / total_thr_energy,
               power_rate, energy_level, m_one, n_one, g_one, m_two, n_two, g_two);
        //schedule(event_doaccelerate, curTick() + accelerator_latency);
        schedule(event_energy_harvest, curTick() + havest_latency);
    }
}
void 
EnergyMGMT::Tile(double times, double delay, double energy, int layer)
{
    EnergyObject::Total_energy -= energy * times * 800 / 1000000000;
    //printf("consume: %f\n", energy * times * 800 / 1000000000);
    if (EnergyObject::Total_energy <= 0){
        EnergyObject::Total_energy = 0;
        break_flag = 1;
        //printf("no enought energy!\n");
    } else {
        total_cal_times  += delay * times;
        current_time     += delay * times;
        total_cal_energy += energy * times * 800 / 1000000000;
         //printf("total_cal_energy%f,%f, %f, %f\n",total_cal_energy, one_cal_time, two_cal_time, EnergyObject::Total_energy);
        // getchar();
        if (current_time >= 100000000000){
            current_time = 0;
            break_flag = 1;
            EnergyObject::Total_energy = 0;
            printf("0.1s time up\n");
        } else {
            if (layer == 1){
                one_cal_time++;
            } else if (layer == 2){
                two_cal_time++;
            }
        }
    }
}
void 
EnergyMGMT::Accelerator(double total_energy, double output_bit, int layer)
{
    double calculation_times  = 0;
    double calculation_delay  = 0;
    double calculation_energy = 0;
    int             tile_size = 0;
    double layer_one_size     = 150;
    double layer_two_size     = 2400;

    if (layer == 1){
        switch (energy_level){// layer 1
            case 0 :
                m = 0;
                n = 0;
                g = 0;                
                calculation_times  = 0;
                calculation_delay  = 0;
                calculation_energy = 0;
                tile_size          = 0;
                //Tile(calculation_times, calculation_delay, calculation_energy, layer);
                throughput_tile = tile_size * 1000000000000 / 800 / 8 / 1024 / 1024;
                power_rate = calculation_energy * 100 / total_energy;
                break;
            case 1 :
                m = 25;
                n = 6;
                g = 1;                
                tile_size = m * n * g;
                calculation_times  = layer_one_size / tile_size;
                calculation_delay  = 800;
                calculation_energy = 1682.95;
                Tile(calculation_times, calculation_delay, calculation_energy, layer);
                throughput_tile = tile_size * 1000000000000 / 800 / 8 / 1024 / 1024;
                power_rate = calculation_energy * 100 / total_energy;
                break;
            default:
                break;
        }
    } else if (layer == 2){
        switch (energy_level){// layer 2
            case 0 :
                m = 0;
                n = 0;
                g = 0;                
                calculation_times  = 0;
                calculation_delay  = 0;
                calculation_energy = 0;
                tile_size          = 0;
                //Tile(calculation_times, calculation_delay, calculation_energy, layer);
                throughput_tile = tile_size * 1000000000000 / 800 / 8 / 1024 / 1024;
                power_rate = calculation_energy * 100 / total_energy;
                break;
            case 1 :
                m = 150;
                n = 16;
                g = 1;                
                tile_size = m * n * g;
                calculation_times = layer_two_size / tile_size;
                calculation_delay  = 800;
                calculation_energy = 4633.70;
                Tile(calculation_times, calculation_delay, calculation_energy, layer);
                throughput_tile = tile_size * 1000000000000 / 800 / 8 / 1024 / 1024;
                power_rate = calculation_energy * 100 / total_energy;
                break;
            default:
            //printf("error!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                break;
        }
    }
    
    //DataTransition();

}

void 
EnergyMGMT::ConsumeEnergy(double energy)
{
    EnergyObject::Total_energy += energy;
    if (EnergyObject::Total_energy >= ENERGY_LIMITATION){
        EnergyObject::Total_energy = ENERGY_LIMITATION;
        printf("it is too big, consume\n");
    }
    DPRINTF(EnergyConsume, "Energy harvest: %d. Total energy remain: %d.\n", 
        energy, EnergyObject::Total_energy);
}

std::vector<double> 
EnergyMGMT::readEnergyProfile() 
{
    std::vector<double> data;
    data.resize(0);
    std::string path_energy_profile = "/home/yizhi/extend/work/gem5"
        //"_new/enenrgy_trails/Solar3";
        "_new/gem5_qiu_b/qiu_trace/RF_trace_one";
        //"_new/enenrgy_trails/RFOffice";
        //"_new/gem5_energy/src/energy/solar_trace";
    double temp;
    std::ifstream fin;
    fin.open(path_energy_profile.c_str());
    assert(fin);    
    while (fin>>temp) {
            data.push_back(temp);
    }
    reverse(data.begin(), data.end());
    fin.close();
    return data;
}
std::vector<double> 
EnergyMGMT::readConsumeTrace() 
{
    std::vector<double> data;
    data.resize(0);

    std::string path_energy_profile = "/home/yizhi/extend/work/gem5"
        //"_new/enenrgy_trails/Solar3";
        "_new/enenrgy_trails/Solar_oneday";
        //"_new/enenrgy_trails/RFOffice";
        //"_new/gem5_energy/src/energy/solar_trace";
    double temp;
    std::ifstream fin;
    fin.open(path_energy_profile.c_str());
    assert(fin);    
    /*while (fin>>temp) {
        data.push_back(temp);
    }

    reverse(data.begin(), data.end());
    fin.close();
    havest_latency = data.back();
    data.pop_back();
    return data;*/
    while (fin>>temp) {
            data.push_back(temp);
    }

    reverse(data.begin(), data.end());
    fin.close();
    //havest_latency = data.back() * 2.25;
    //data.pop_back();
    //printf("read done. harvest latency is: %f\n", havest_latency);
    return data;
}
void 
EnergyMGMT::EnergyHarvest() 
{
    if (!energy_harvest_data.empty()){
        double energy_val = energy_harvest_data.back();
        ConsumeEnergy(energy_val * 0.1 * 1000);
        if (1 == 1){
            if (energy_val >= 0 && energy_val <= 4633.7){
                energy_level = 0;
            } else if (energy_val > 4633.7 ){
                energy_level = 1;
            }
        }
    //     printf("current input power level:%d, input energy:%f, current tick is: %f\n", 
    //    energy_level, energy_val * 0.1 * 1000, (double)curTick());
        energy_harvest_data.pop_back();
        power_cycle++;
        if (EnergyObject::Total_energy >= ENERGY_LIMITATION) {
            EnergyObject::Total_energy = ENERGY_LIMITATION;
            printf("it is too big\n");
        }
        DPRINTF(EnergyConsume, "harvested : %d. Total energy remain: %lf.\n", 
        energy_val, EnergyObject::Total_energy);
        //schedule(event_energy_harvest, curTick() + havest_latency);
        schedule(event_doaccelerate, curTick() + accelerator_latency);
    } else {
        DPRINTF(EnergyConsume, "NO MORE ENERGY CAN BE HARVESTED! Current energy is %d.\n", 
            EnergyObject::Total_energy);
        DPRINTF(EnergyStateMachine,"NO MORE ENERGY CAN BE HARVESTED! Current energy is %d.\n",
            EnergyObject::Total_energy);
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        //assert(!energy_harvest_data.empty());
        exitSimLoop("power off !");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        //assert(!energy_harvest_data.empty());
    }
}

EnergyMGMT*
EnergyMGMTParams::create()
{
    return new EnergyMGMT(this);
}
