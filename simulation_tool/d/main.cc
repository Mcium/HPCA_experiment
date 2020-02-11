#include "param_read.hh"
#include "core.hh"

#define D_WRITE 1
#define D_READ  0

int 
main()
{
    //for data processing
    long long int statis_total_times            = 0;
    double        statis_total_input_energy     = 0;
    long long int statis_total_work_times       = 0;
    long long int statis_total_mid_work_times   = 0;
    long long int statis_total_add_work_times   = 0;
    long long int statis_total_c_work_times     = 0;
    double        statis_total_large_enr_times  = 0;
    double        statis_total_enr_rate         = 0;
    double        statis_total_enr_rate_con     = 0;
    double        statis_total_enr_rate_cal     = 0;
    double        statis_total_enr_rate_trans   = 1;
    long long int statis_total_rate_times       = 0;
    long long int statis_thr_per_sec            = 0;
    long long int statis_thr_per_J              = 0;
    //data processing parameters declare end
    int layer_num         = 0;
    double total_energy   = 0;
    double threshold      = 83.3;
    std::string trace;
    std::string method;
    std::string network;
    std::string pathf;
    std::string final_file;

    ReadData read_obj;
    //input trace name
    std::cout << "input trace file name (piezo, solar, thermal, tvrf, wifi-h, wifi-o):";
    std::cin >> trace;
    std::cout << "trace file name is:" << trace << std::endl;

    double power_space = 0;
    if (trace == "piezo") {
        power_space = 0.0001;
    } else if (trace == "thermal" || trace == "wifi-h" || trace == "wifi-o") {
        power_space = 0.2;
    } else if (trace == "solar") {
        power_space = 60;
    } else if (trace == "tvrf") {
        power_space = 0.1;
    }
    assert(trace == "piezo" || trace == "thermal" || trace == "wifi-h" 
           || trace == "wifi-o" || trace == "solar" || trace == "tvrf");

    std::string predictor;
    std::cout << "predictor? (y or n):";
    std::cin >> predictor;

    assert(predictor == "y" || predictor == "n");


    //input mehod
    std::cout << "input method(only d):";
    std::cin >> method;
    std::cout << "current method is:" << method << std::endl;
    assert(method == "d");
    //input network name
    std::cout << "input network name (lenet, pv, fr, hg):";
    std::cin >> network;
    std::cout << "current network is:" << network << std::endl;
    pathf = "gchange/" + network + "_" + trace + "/";


    if (predictor == "n"){
        pathf = "data/" + network + "_" + trace + "/";
    } else if (predictor == "y"){
        pathf = "data/predictor/" + network + "_" + trace + "/";
    }


    if (network == "lenet") {
        layer_num = 2;
        std::cout << "layer number is:" << layer_num << std::endl;
    } else if (network == "pv") {
        layer_num = 5;
        std::cout << "layer number is:" << layer_num << std::endl;
    }else if (network == "fr") {
        layer_num = 2;
        std::cout << "layer number is:" << layer_num << std::endl;
    }else if (network == "hg") {
        layer_num = 2;
        std::cout << "layer number is:" << layer_num << std::endl;
    }
    double layer_times[layer_num];
    int    kernel_size[layer_num];
    int full_tile_size[layer_num];
    double network_cal_times = 0;
    //read calculation times for per payer
    if (network == "lenet") {
        layer_times[0]    = 784;
        layer_times[1]    = 100;
        kernel_size[0]    = 25;
        kernel_size[1]    = 150;
        full_tile_size[0] = 150;
        full_tile_size[1] = 2400;
        network_cal_times = 357600;

    } else if (network == "pv") {
        layer_times[0]    = 2025;
        layer_times[1]    = 400;
        layer_times[2]    = 64;
        layer_times[3]    = 36;
        layer_times[4]    = 16;
        kernel_size[0]    = 36;
        kernel_size[1]    = 72;
        kernel_size[2]    = 108;
        kernel_size[3]    = 144;
        kernel_size[4]    = 90;
        full_tile_size[0] = 288;
        full_tile_size[1] = 864;
        full_tile_size[2] = 1728;
        full_tile_size[3] = 1440;
        full_tile_size[4] = 540;
        network_cal_times = 1616544;
    }else if (network == "fr") {
        layer_times[0]    = 784;
        layer_times[1]    = 100;
        kernel_size[0]    = 25;
        kernel_size[1]    = 64;
        full_tile_size[0] = 100;
        full_tile_size[1] = 1024;
        network_cal_times = 180800;
    }else if (network == "hg") {
        layer_times[0]    = 576;
        layer_times[1]    = 64;
        kernel_size[0]    = 25;
        kernel_size[1]    = 96;
        full_tile_size[0] = 150;
        full_tile_size[1] = 1152;
        network_cal_times = 160128;
    }
    int tile_delay[layer_num];
    for (int i = 1; i < layer_num + 1; i++) {
        // std::cout << "input tile delay (ps) of layer " << i << " :";
        // std::cin  >> tile_delay[i - 1];
        tile_delay[i - 1] = 80000;
    }
    

    int max_layer_times = 0;
    for (int i = 0; i < layer_num; i++) {
        int mid_layer_times = layer_times[i];
        if (mid_layer_times >= max_layer_times){
            max_layer_times = mid_layer_times;
        }
    }

    std::vector<double> Energy_trace;
    std::vector<double> Predictor_trace;
    if (predictor == "n"){
        Energy_trace    = read_obj.read_single(pathf + trace, 1);
    } else if (predictor == "y"){
        Energy_trace    = read_obj.read_single(pathf + trace + "_0", 1);
        Predictor_trace = read_obj.read_single(pathf + trace + "_1", 1);
    }
    std::cout << "read done energy trace" << std::endl;
    std::vector<double> M_data[layer_num];
    std::vector<double> N_data[layer_num];
    std::vector<double> G_data[layer_num];
    std::vector<double> Range_data_l;
    std::vector<double> Range_data_h;
    std::vector<double> Power_data[layer_num];

    Range_data_l    = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", 0, layer_num);
    Range_data_h    = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", 1, layer_num);
    for(int i = 0; i < layer_num; i++){
        M_data[i]       = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", (i + 2) + (i * 3), layer_num);
        N_data[i]       = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", (i + 3) + (i * 3), layer_num);
        G_data[i]       = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", (i + 4) + (i * 3), layer_num);
        Power_data[i]   = read_obj.read_all(pathf + network + "_" + method + "_data" + ".csv", (i + 5) + (i * 3), layer_num);
    }
    std::cout << "read done detail, current size is:" << Range_data_h.size() << std::endl;
    assert(Range_data_l.size() == Range_data_h.size()); //check range data
    for(int i = 0; i < layer_num; i++){
        assert(M_data[i].size() == N_data[i].size());
        assert(G_data[i].size() == N_data[i].size());
        assert(G_data[i].size() == Power_data[i].size());
    }

    final_file = pathf + "final_result_" + trace + "_" + network + "_" + method + ".csv";
    remove(final_file.c_str());
    std::cout << "start simulation." << std::endl;
//  initialization done, start progress.==============================================================
    int power_cycle           = 0;
    double middle_m           = 0;
    double middle_n           = 0;
    double middle_g           = 0;
    double middle_layer_times = 0;
    int    middle_layer_num   = 0;
    double middle_prop        = 0;
    int    last_level         = 0;
    double last_prop          = 0;
    while(!Energy_trace.empty()){
        int    work_times        = 0;
        int    prediction_times  = 0;
        double cal_rate          = 0;
        double total_rate        = 0;
        int    energy_level      = 0;
        double middle_work_times = 0;
        double current_energy    = Energy_trace.back();
        double predictor_result  = 0;
        if (predictor == "y") {
            predictor_result = Predictor_trace.back();
            Predictor_trace.pop_back();
        }
        for (int j = 0; j < Range_data_h.size(); j++) {
            if (current_energy >= Range_data_l[j] && current_energy <= Range_data_h[j]) {//out of bound
                energy_level = j;
                break;
            }
            energy_level = 0;
        }
        // std::cout << "input: power:" << current_energy << ", " << energy_level;
        current_energy = current_energy * 1000 * power_space; //normalization unit
        int restart_times = 0;
        int restart_layer = 0;
        AcceleratorCore *core = new AcceleratorCore(current_energy, power_space);
        //find max calculation times
        double max_cal_times = 0;
        int final_layer_num = 0;
        for (int i = 0; i < layer_num; i++) {
            double mid_cal_times = 0;
            mid_cal_times = core->CalculationTime(full_tile_size[i], M_data[i][energy_level], 
                            N_data[i][energy_level], G_data[i][energy_level]);
            if(mid_cal_times >= max_cal_times){
                final_layer_num = i;
                max_cal_times = mid_cal_times;
            }

        }
        double max_m = 0;
        double max_n = 0;
        double max_g = 0;
        for (int i = 0; i < layer_num; i++) {

            if(M_data[i][energy_level] >= max_m){
                max_m = M_data[i][energy_level];
            }
            if(N_data[i][energy_level] >= max_n){
                max_n = N_data[i][energy_level];
            }
            if(G_data[i][energy_level] >= max_g){
                max_g = G_data[i][energy_level];
            }
        }
        //check middle work
        if (middle_m == M_data[middle_layer_num][energy_level] &&
            middle_g == G_data[middle_layer_num][energy_level]){
            restart_times = middle_prop * layer_times[middle_layer_num];
            restart_layer = middle_layer_num;

            for(int i = restart_layer; i < 1; i++){
                core->DataTransaction(kernel_size[i], D_READ);
                for(int j = restart_times; j < max_layer_times; j++){
                    if (core->DoAccelerate(tile_delay[i], full_tile_size[i], max_m, max_n,
                        max_g, Power_data[i][energy_level], max_cal_times)){
                        middle_m           = M_data[i][energy_level];
                        middle_n           = N_data[i][energy_level];
                        middle_g           = G_data[i][energy_level];
                        middle_layer_times = j;
                        middle_layer_num   = i;
                        middle_prop        = (double)j / layer_times[i];
                        goto quit;  
                    }
                }
                core->DataTransaction(kernel_size[i], D_WRITE);
            }
            work_times++;
            middle_work_times++;
        }
        // do simulation
        while(energy_level != 0){
            for(int i = 0; i < 1; i++){
                core->DataTransaction(kernel_size[i], D_READ);
                for(int j = 0; j < max_layer_times; j++){
                    if (core->DoAccelerate(tile_delay[i], full_tile_size[i], max_m, max_n,
                        max_g, Power_data[i][energy_level], max_cal_times)){
                        middle_m           = M_data[i][energy_level];
                        middle_n           = N_data[i][energy_level];
                        middle_g           = G_data[i][energy_level];
                        middle_layer_times = j;
                        middle_layer_num   = i;
                        middle_prop        = (double)j / layer_times[i];
                        goto quit;  
                    }
                }
                core->DataTransaction(kernel_size[i], D_WRITE);
            }
            work_times++;
        }
    quit:
        //addtional work
        if (predictor == "y") {
            if(last_level > energy_level){
                if(predictor_result == 1){
                    if(middle_prop + last_prop >= 1){
                        work_times++;
                        prediction_times++;
                    }
                }
            }
        }
        //end simulation
        last_prop = middle_prop;
        last_level = energy_level;
        cal_rate = Power_data[0][energy_level]/(current_energy/(1000 * power_space));
        if(energy_level != 0){
            cal_rate   = cal_rate/1;
            total_rate = (cal_rate + 1) / 2;
        } else {
            cal_rate   = 0;
            total_rate = 0;
        }
        if((((current_energy/(1000 * power_space)) > threshold)?1:0) == 1){
            statis_total_large_enr_times++;
        }
        
        Energy_trace.pop_back();
        
        power_cycle++;
        delete core;
        
        std::ofstream outfile;
        outfile.open(final_file, std::ios::app);
        outfile << power_cycle << "," << current_energy/(1000 * power_space) << "," << work_times << 
            "," << middle_work_times << "," << prediction_times  << "," << cal_rate << "," << total_rate << "," << energy_level;
        for (int i = 0; i < layer_num; i++) {
            outfile << "," << M_data[i][energy_level] << "*" << N_data[i][energy_level]
                << "*" << G_data[i][energy_level];
        }
        outfile << std::endl;
        outfile.close();
        statis_total_input_energy   += current_energy/(1000 * power_space);
        statis_total_work_times     += work_times;
        statis_total_mid_work_times += middle_work_times;
        statis_total_add_work_times += prediction_times;
        if(total_rate != 0 && cal_rate != 0){
            statis_total_enr_rate_cal += cal_rate;
            statis_total_enr_rate     += total_rate;
            statis_total_enr_rate_con++;
        }

    }
    //final result
    statis_total_times        = power_cycle * power_space;
    statis_total_input_energy = statis_total_input_energy * statis_total_times / 1000000;
    statis_thr_per_sec        = network_cal_times * statis_total_work_times / statis_total_times;
    statis_thr_per_J          = network_cal_times * statis_total_work_times / statis_total_input_energy;

    std::ofstream outfile;
    outfile.open(final_file, std::ios::app);
    outfile << "total work times: ," << statis_total_work_times 
            << "," << "total middle work times: ," << statis_total_mid_work_times 
            << "," << "addition work time: ," << statis_total_add_work_times 
            << "," << "c work time: ," << statis_total_c_work_times << std::endl;
    outfile << "input power larger than " << threshold << " times and proportion: ," << statis_total_large_enr_times 
            << "," << statis_total_large_enr_times / power_cycle
            << "," << "power utilization(cal): ," << statis_total_enr_rate_cal / statis_total_enr_rate_con
            << "," << "power utilization(trans): ," << statis_total_enr_rate_trans
            << "," << "power utilization(total): ," << statis_total_enr_rate / statis_total_enr_rate_con << std::endl;
    outfile << "power utilization(cal when input power larger than threshold): ," 
            << statis_total_enr_rate_cal / statis_total_large_enr_times
            << "," << "power utilization(total when input power larger than threshold): ," 
            << statis_total_enr_rate / statis_total_large_enr_times << std::endl;
    outfile << "throughput(per sec): ," << statis_thr_per_sec << ","
        << "throughput(per J): ," << statis_thr_per_J << std::endl;
    outfile.close();
    return 0;
}