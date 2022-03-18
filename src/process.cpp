#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id): m_pid(id){}

// DONE: Return this process's ID
int Process::Pid() { return m_pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
    long totalTime = LinuxParser::ActiveJiffies(m_pid);
    long hertz = sysconf(_SC_CLK_TCK);

    long sys_uptime = LinuxParser::UpTime();
    long proc_startTime =  LinuxParser::UpTime(m_pid);
    long proc_started = sys_uptime - ((float)proc_startTime / (float) hertz);

    float totaltime_s = ((float)totalTime / (float)hertz);

    // apply simple delay filter on K1 values to avoid zeros in _dt values
//    m_uptime_s_K1.emplace_back(proc_started);
//    m_totaltime_s_K1.emplace_back(totaltime_s);
//
//    float up_dt;
//    float tot_dt;
//    if (m_uptime_s_K1.size() > 1) {
//        up_dt = proc_started - m_uptime_s_K1[0];
//        tot_dt = totaltime_s - m_totaltime_s_K1[0];
//    } else {
//        up_dt = proc_started;
//        tot_dt = totaltime_s;
//    }
//    
//
//    if(m_uptime_s_K1.size() >= 5){
//        m_uptime_s_K1.erase(m_uptime_s_K1.begin());
//        m_totaltime_s_K1.erase(m_totaltime_s_K1.begin());
//    }
//
//    // ensure to not divide by 0
//    if (up_dt == 0) {    
//        up_dt = 1;
//    }
//    float util = (tot_dt / up_dt);
    float util = (totaltime_s / proc_started);
    m_cpu = util; // store for faster accesses of comparision function
    return util;
 }

// DONE: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(m_pid);
 }

// DONE: Return this process's memory utilization
string Process::Ram() { 
    string ram = LinuxParser::Ram(m_pid);
    if (ram.size() > 3){
        ram.resize(ram.size() - 3); // convert from kB to MB
    } else {
        ram = "0";
    }
    return ram;
 }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(m_pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { 
    long hertz = sysconf(_SC_CLK_TCK);
    long startTime =  LinuxParser::UpTime(m_pid);
    long upTime = LinuxParser::UpTime();
    long int seconds = upTime - (startTime / hertz);
    return seconds;
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return a.m_cpu < m_cpu;
    //return m_pid < a.m_pid;
 }

// returns true if the process is part of the provided pid list, false otherwise
bool Process::Alive(std::vector<int> pids){
    auto it = std::find(pids.begin(), pids.end(), m_pid);
    return it != pids.end();
}