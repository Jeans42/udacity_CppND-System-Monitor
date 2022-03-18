#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace LinuxParserHelp {
  string SearchForKeyInFile_line(string in_filepath, string in_key) {
    // searches for the key in the provided file and returns the line
    string line, key;
    std::ifstream stream(in_filepath);
    if (stream.is_open()){
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key) {
          if (key == in_key) {
            return line;
          }
        }
      }
    }
    return line;
  }

  // searches for the key in the provided file and returns the value
  string SearchForKeyInFile_value(string in_filepath, string in_key) {
    string line, key, value;

    line = SearchForKeyInFile_line(in_filepath, in_key);
    std::istringstream linestream(line);
    linestream >> key >> value;
    return value;
  }

  long SearchForLongInLine(string line){
    long ret;
    size_t pos;
    string value;

    while ((pos = line.find(" ")) != string::npos) {
      value = line.substr(0, pos);
      line.erase(0, pos + 1);
      if ( std::all_of(value.begin(), value.end(), ::isdigit) && value != "") {
        ret = stol(value);
      }
    }
    return ret;
  }

}


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  long memtotal, memavail, memfree;
  string line, value;

  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + kMeminfoFilename, "MemTotal:");
  memtotal = LinuxParserHelp::SearchForLongInLine(line);
  
  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + kMeminfoFilename, "MemAvailable:");
  memavail = LinuxParserHelp::SearchForLongInLine(line);

  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + kMeminfoFilename, "MemFree:");
  memfree = LinuxParserHelp::SearchForLongInLine(line);

  return (float)(memavail - memfree) / (float)memtotal;
 }

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idle, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
    
  }
  return std::stol(uptime, nullptr, 10);
 }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string utime, stime, cutime, cstime, value;
  size_t pos;
  string line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + to_string(pid) + kStatFilename, to_string(pid));
  if (line != "") {
    int i = 0;
      while ((pos = line.find(" ")) != string::npos && i <= 17) {
        value = line.substr(0, pos);
        line.erase(0, pos + 1);
        ++i;
        switch (i)
        {
          case 14:
            utime = value;
            break;
          case 15:
            stime = value;
            break;
          case 16: 
            cutime = value;
            break;
          case 17:
            cstime = value;
            break;
          default:
            break;
        }
      }
      return std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
  } else {
    // line is empty. Might happen if the process is terminated between reading the pids and this function
    return 0; 
  }
  
 }

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> values;
  string cpu, value;
  string line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + kStatFilename, "cpu");
  size_t pos = 0;
  // split line at each space and if all chars are a digit put it to output
  while ((pos = line.find(" ")) != string::npos) {
    value = line.substr(0, pos);
    line.erase(0, pos + 1);
    if ( std::all_of(value.begin(), value.end(), ::isdigit) && value != "") {
      values.push_back(value);
    }
  }
  return values;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string value;
  value = LinuxParserHelp::SearchForKeyInFile_value(kProcDirectory + kStatFilename, "processes");
  std::ifstream stream(kProcDirectory + kStatFilename);
  return std::stoi(value);
 }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {   
  string value;
  value = LinuxParserHelp::SearchForKeyInFile_value(kProcDirectory + kStatFilename, "procs_running");
  std::ifstream stream(kProcDirectory + kStatFilename);
  return std::stoi(value);
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + std::to_string(pid) + kCmdlineFilename, "");
  return line;
}

// DONE: Read and return the memory used by a process in kB
string LinuxParser::Ram(int pid) { 
  string line, temp, Uid, value, kB;
  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:");
  std::replace(line.begin(), line.end(), '\t', ' ');
  std::istringstream linestream(line);
  linestream >> temp >> value >> kB;
  return value;
 }

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, temp, Uid;
  line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + std::to_string(pid) + kStatusFilename, "Uid:");
  std::replace(line.begin(), line.end(), '\t', ' ');
  std::istringstream linestream(line);
  linestream >> temp >> Uid;

  return Uid;
 }

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, user, temp, Uid, id;
  Uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> temp >> id) {
        if (id == Uid) {
          return user;
        }
      }
    }
  }
  return user;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string value;
  size_t pos;
  string line = LinuxParserHelp::SearchForKeyInFile_line(kProcDirectory + to_string(pid) + kStatFilename, to_string(pid));
  int i = 0;
  while ((pos = line.find(" ")) != string::npos) {
    value = line.substr(0, pos);
    line.erase(0, pos + 1);
    ++i;
    if (i >= 22) {
      return std::stol(value);
    }
  }
  return 0; // not happening because while loop goes on and on until i is 22
}

