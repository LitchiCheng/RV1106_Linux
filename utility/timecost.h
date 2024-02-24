#include <sys/time.h>
#include <time.h>
#include <iostream>

class timecost
{
private:
    struct timeval _start;
    struct timeval _end;
    std::string _name;
    float time_diff(struct timeval *start, struct timeval *end) {
        return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
    }
public:
    timecost(std::string item_name):_name(item_name){
        gettimeofday(&_start, NULL);
    }
    ~timecost(){
        gettimeofday(&_end, NULL);
        std::cout << _name << "time spent:" << time_diff(&_start, &_end) << "sec" << std::endl;
    }
};


