#include "processor.h"
#include "linux_parser.h"

using namespace LinuxParser;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    // read from file system from Linuxparser
    std::vector<std::string> data = CpuUtilization();
    
    // interpret the data
    unsigned long idle = std::stoi(data[kIdle_]) + std::stoi(data[kIOwait_]);
    unsigned long nonidle = std::stoi(data[kUser_]) + std::stoi(data[kNice_]) + std::stoi(data[kSystem_])\
                 + std::stoi(data[kIRQ_]) +std::stoi(data[kSoftIRQ_])+ std::stoi(data[kSteal_]);
    unsigned long total = idle + nonidle;

    // get dt values
    unsigned long total_dt = total - total_K1;
    unsigned long idle_dt = idle - idle_K1;

    // store K1 values
    total_K1 = total;
    idle_K1 = idle;
    
    if (total_dt == 0) {
        // prevent division by 0, set CPU to 100% in this case.
        // should not happen at all since at least some numbers are written in the stat file
        total_dt = 1;
        idle_dt = 0;
    }
    float result =  ((float)(total_dt - idle_dt)) / (float)total_dt;
    return result;
}
