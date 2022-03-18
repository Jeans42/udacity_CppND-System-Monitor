#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    // get all current Pids, compare to old and create new process objects if needed
    vector<int> pids_new = LinuxParser::Pids();
    vector<int> idx_toRemove = {};

    

    for (size_t i = 0; i < processes_.size(); i++) {
        if (!processes_[i].Alive(pids_new)){
            // process terminated, remove from list
            processes_.erase(processes_.begin() + i);
        } else {
            // process still alive, find it's pid and remove it from the new pid list
            auto it = std::find(pids_new.begin(), pids_new.end(), processes_[i].Pid());
            pids_new.erase(it);
        }
    }

    // add new processes
    for (int pid : pids_new) {
        processes_.emplace_back(Process(pid));
    }
    
    // filter processes: show only procs with command, RAM usage
    proc_relevant.clear();
    for (auto proc : processes_) {
        if (proc.Command() != "" && proc.Ram() != "0"){
            proc_relevant.emplace_back(proc);
        }
    }

    // sort the processes by CPU load
    std::sort(proc_relevant.begin(), proc_relevant.end());
    //std::reverse(proc_relevant.begin(), proc_relevant.end());
    return proc_relevant; 
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }