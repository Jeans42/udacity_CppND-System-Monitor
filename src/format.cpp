#include <string>
#include <sstream>
#include <iomanip> 

#include "format.h"

using std::string;

// converts from long to HH:MM:SS string format
string Format::ElapsedTime(long seconds) { 
    int h, min, sec;
    
    h = seconds / (60 * 60);
    seconds = seconds - h * 60 * 60;

    min = seconds / 60;
    sec = (int)(seconds - min * 60);

    std::stringstream sh, sm, ss;
    sh << std::setw(2) << std::setfill('0') << h;
    sm << std::setw(2) << std::setfill('0') << min;
    ss << std::setw(2) << std::setfill('0') << sec;
    
    return sh.str() + ":" + sm.str() + ":" + ss.str();
}