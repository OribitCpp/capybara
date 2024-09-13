#ifndef  LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <memory>

class Logger {
	friend class Application;
private:
	Logger();
public:
	~Logger();
    template <typename... Args>
	static void debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		m_logger->debug(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		m_logger->info(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		m_logger->warn(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		m_logger->error(fmt, std::forward<Args>(args)...);
	}
private:
	static std::shared_ptr<spdlog::logger> m_logger;
};

#endif // ! LOGGER_H
