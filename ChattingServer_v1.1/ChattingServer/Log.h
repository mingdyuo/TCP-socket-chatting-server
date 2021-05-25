#pragma once

#ifndef LOG_H
#define LOG_H


#include <iostream>
#include <sstream>
#include <filesystem>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "unistd.h"

#define LOG_PATH "./log/"
#define LOGGER_NAME "logger"
#define GET_LOGGER spdlog::get(LOGGER_NAME)

class LOG
{
public:
	LOG(const std::string& filename) :
		loggerDropeed_(false),
		logFileName_(filename)
	{
		this->Initialize();
	}

	~LOG()
	{
		this->dropLog();
	}

	void dropLog()
	{
		if (!loggerDropeed_)
		{
			spdlog::drop(LOGGER_NAME);
			loggerDropeed_ = true;
		}
	}


private:

	void Initialize()
	{
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(this->createLogPath()));
		auto combined_logger = std::make_shared<spdlog::logger>(LOGGER_NAME, begin(sinks), end(sinks));

		spdlog::register_logger(combined_logger);
		spdlog::set_pattern("[%Y-%m-%d %T] [thread %t] [%l] %v");

		loggerDropeed_ = false;
	}

	std::string createLogPath()
	{
		std::stringstream logFullPath;
		logFullPath << LOG_PATH << logFileName_;
		if (access(LOG_PATH, F_OK) != 0)
		{
			std::filesystem::create_directory(LOG_PATH);
		}
		return logFullPath.str();
	}

private:
	bool loggerDropeed_;

	std::string logFileName_;
	std::vector<spdlog::sink_ptr> sinks;
};




#endif // !LOG_H