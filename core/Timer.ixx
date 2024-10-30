export module Core:Timer;

import <ctime>;
import <chrono>;
import <sstream>;
import <iomanip>;
import <string>;

export class Timer {
public:
    Timer() = delete;
    Timer(const Timer&) = delete;

	// return empty
	static std::string YMDHMS() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));

        //std::ostringstream oss;
        return ;
	}
};


