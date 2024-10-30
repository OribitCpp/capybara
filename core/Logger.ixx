export module Logger;

import <spdlog/spdlog.h>;
import <spdlog/common.h>;
import <spdlog/sinks/basic_file_sink.h>;
import <spdlog/sinks/stdout_color_sinks.h>;
import <memory>;
import Timer;


export class Logger {
	friend class Application;
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger(const Logger&&) = delete;
	Logger& operator=(const Logger&&) = delete;

	static Logger& instance() {
		static Logger logger;
		return logger;
	}

private:
	Logger() {
		//std::string fileName = "logs/" + Timer::YMDHMS() + ".txt";
		//m_logger = spdlog::basic_logger_mt("Logger", fileName);
		m_logger = spdlog::stdout_color_mt("Logger");
		m_logger->set_level(spdlog::level::trace);
		m_logger->flush_on(spdlog::level::trace);
		m_logger->info("logger is created  success");
	}
public:
	~Logger() {

	}
    template <typename... Args>
	static void debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		instance().m_logger->debug(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		instance().m_logger->info(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		instance().m_logger->warn(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		instance().m_logger->error(fmt, std::forward<Args>(args)...);
	}
private:
	static std::shared_ptr<spdlog::logger> m_logger;
};


std::shared_ptr<spdlog::logger> Logger::m_logger;
