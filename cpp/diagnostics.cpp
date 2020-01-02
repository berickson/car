#include "diagnostics.h"
#include "logger.h"

using namespace std;
using namespace std::chrono;

PerformanceData::PerformanceData(const char * name) : name(name) {
    _construct_time = system_clock::now();
}


MethodTracker::MethodTracker(PerformanceData & data) : _data(data) {
_start_time = system_clock::now();
}

MethodTracker::~MethodTracker() {
    auto end_time = system_clock::now();
    _data.total_duration += (end_time-_start_time);
    ++(_data.call_count);
    if((_data.call_count % _data.report_every_n_calls) == 0) {
        auto clock_elapsed = system_clock::now() - _data._construct_time;
        float percent_wall = 100. * _data.total_duration.count() / clock_elapsed.count();
        log_info(
        _data.name 
        + " call_count: " 
        + to_string(_data.call_count) 
        + " total_duration: " 
        + to_string(_data.total_duration.count() / 1E6) + " ms"
        + " average_duration: " 
        + to_string((_data.total_duration.count() / 1E6) / _data.call_count)+ " ms"
        + "% wall: " + to_string(percent_wall)
        );
    }
}

