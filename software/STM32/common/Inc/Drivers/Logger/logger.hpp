/*
 * Logger.hpp
 *
 *  Created on: Sep 18, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "Drivers/Logger/usb_serial_port.hpp"
#include <string_view>
#include <utility>

#define FMT_HEADER_ONLY
#include "fmt/core.h"


class Logger {
public:
	Logger(USBSerialPort& serial_port) : serial_port{serial_port} {}

	template<typename ...Args>
	void info(fmt::format_string<Args...> fmt, Args&&... args);

	template<typename ...Args>
	void warn(fmt::format_string<Args...> fmt, Args&&... args);

	template<typename ...Args>
	void error(fmt::format_string<Args...> fmt, Args&&... args);

	template<typename ...Args>
	void gcs_sensor(fmt::format_string<Args...> fmt, Args&&... args);

	template<typename ...Args>
	void gcs_state(fmt::format_string<Args...> fmt, Args&&... args);

private:
	USBSerialPort& serial_port;

	static constexpr std::string_view INFO			= "INFO";
	static constexpr std::string_view WARNING		= "WARN";
	static constexpr std::string_view ERROR			= "ERROR";
	static constexpr std::string_view SENSOR		= "SENSOR";
	static constexpr std::string_view STATE			= "STATE";

	void print_log_with_timestamp(std::string&& log, std::string_view severity);
};


/*
 *
 * TEMPLATE IMPLEMENTATION
 *
 */
template<typename ...Args>
void Logger::info(fmt::format_string<Args...> fmt, Args&&... args)
{
	print_log_with_timestamp(fmt::format(fmt, std::forward<Args>(args)...), Logger::INFO);
}

template<typename ...Args>
void Logger::warn(fmt::format_string<Args...> fmt, Args&&... args)
{
	print_log_with_timestamp(fmt::format(fmt, std::forward<Args>(args)...), Logger::WARNING);
}

template<typename ...Args>
void Logger::error(fmt::format_string<Args...> fmt, Args&&... args)
{
	print_log_with_timestamp(fmt::format(fmt, std::forward<Args>(args)...), Logger::ERROR);
}

template<typename ...Args>
void Logger::gcs_sensor(fmt::format_string<Args...> fmt, Args&&... args)
{
	print_log_with_timestamp(fmt::format(fmt, std::forward<Args>(args)...), Logger::SENSOR);
}

template<typename ...Args>
void Logger::gcs_state(fmt::format_string<Args...> fmt, Args&&... args)
{
	print_log_with_timestamp(fmt::format(fmt, std::forward<Args>(args)...), Logger::STATE);
}
