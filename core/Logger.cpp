#include "Logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Timer.h"

std::shared_ptr<spdlog::logger> Logger::m_logger;

Logger::Logger()
{
	std::string fileName = "logs/" + Timer::YMDHMS() + ".txt";
	m_logger = spdlog::basic_logger_mt("Logger", fileName);
	m_logger->set_level(spdlog::level::trace);
	m_logger->flush_on(spdlog::level::trace);
	m_logger->info("logger is created  success");
}

Logger::~Logger()
{
}