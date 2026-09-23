/*
 * logger.cpp
 *
 *  Created on: Sep 21, 2026
 *      Author: Nikolai Philipenko
 */

#include "Drivers/Logger/logger.hpp"
#include "cmsis_os.h"
#include "fmt/color.h"

/*
 *
 * PRIVATE
 *
 */
void Logger::print_log_with_timestamp(std::string&& log, std::string_view severity)
{
	float timestamp = static_cast<float>(osKernelGetTickCount()) / static_cast<float>(osKernelGetTickFreq());

	std::string output_string = fmt::format("[{}] [{}] {}\n", timestamp, severity, log);
	std::vector<uint8_t> buffer{output_string.begin(), output_string.end()};
	serial_port.write_bytes(std::move(buffer));
}
