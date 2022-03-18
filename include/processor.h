#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

class Processor {
 public:
  float Utilization(); 

 private:
    unsigned long idle_K1 = 0;
    unsigned long total_K1 = 0;
};

#endif