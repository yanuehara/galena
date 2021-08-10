#pragma once

#include <iomanip>
#include <ctime>
#include <chrono>

#include "singletonLogger.h"

using namespace std;

namespace galena{
    #ifdef GALENADEBUG
        #define GALENATIMEBEGIN std::chrono::time_point<std::chrono::steady_clock> t_start = std::chrono::steady_clock::now();
        #define GALENATIMEEND(func) auto t_end = std::chrono::steady_clock::now(); stringstream ss; auto debug_logger = SingletonLogger::getInstance(); ss << "Function " << func << " took " << (t_end - t_start) / std::chrono::milliseconds(1); debug_logger->writeEntry(ss.str());
    #endif
}