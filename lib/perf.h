/**
 * @file perf.h
 * @author Ayush Sinha
 * @brief Library for timing c++ codes.
 * @version 0.1
 * @date 2022-08-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __PERF_H__
#define __PERF_H__

#include <iostream>
#include <chrono>
#include <utility>
#include <random>

#define perf_clock std::chrono::high_resolution_clock::time_point
#define perf_time std::chrono::duration<double>
#define perf_ms_time std::chrono::duration<double, std::milli>
#define perf_ns_time std::chrono::duration<double, std::nano>


namespace perf {

std::mt19937_64 rng;
perf_clock tmp_clock_startpoint;

inline void randomize()
{ perf::rng.seed(std::chrono::steady_clock::now().time_since_epoch().count()); }

inline perf_clock now()
{ return std::chrono::high_resolution_clock::now(); }

inline void start_clock()
{ tmp_clock_startpoint = now(); }

inline double end_clock() {
    perf_time dur = now() - tmp_clock_startpoint;
    return dur.count();
}


struct Timer {

    perf_clock _start, _end;
    perf_time duration;
    std::string func_name;

    Timer() { 
        func_name = "Timer";
        _start = now();
    }

    Timer(std::string function_name) {
        func_name = function_name;
        _start = now();
    }

    ~Timer() {
        _end = now();
        duration = _end - _start;
        std::cout << func_name << " took " << duration.count() << " sec." << std::endl;
    }


};

/**
 * @brief Returns the time_elpased by a function
 * 
 * Not applicable for template or non-static member functions.
 * 
 * @tparam _Callable 
 * @tparam _Args 
 * @param __f function
 * @param __args arguments of the functions
 * @return func_time (in sec.)
 */
template <typename _Callable, typename... _Args>
double Time(const _Callable &__f, _Args&&... __args) {
    const perf_clock start = now();

    __f(std::forward<_Args>(__args)...);
    
    const perf_time dur = now() - start;
    return dur.count();
}


/**
 * @brief Returns the value & time_elapsed by func. as a pair.
 * 
 * Not applicable for void, template or non-static member functions. (use perf::Time instead for voids)
 * 
 * @tparam _Callable 
 * @tparam _Args 
 * @param __f function
 * @param __args arguments of the functions
 * @return pair( func_return_value, func_time(in sec.) )
 */
template <typename _Callable, typename... _Args>
const auto run_algo(const _Callable &__f, _Args&&... __args) {
    const perf_clock start = now();

    const auto ret_val = __f(std::forward<_Args>(__args)...);
    
    const perf_time dur = now() - start;
    return std::make_pair(ret_val, dur.count());
}


}


#endif



