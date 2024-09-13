#include "Timer.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string Timer::YMDHMS()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
    std::tm* now_tm = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(now_tm, "%Y-%m-%d-%H-%M-%S");
    return oss.str();
}
