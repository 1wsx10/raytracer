#pragma once

#include <chrono>
#include <cstdio>
#include <string>
#include <iostream>
#include <memory>

#include "logger.hpp"

class timer {
	static unsigned int count;

	const char* name;
	enum TYPE {
		PRINTING,
		LOGGING
	} type;
	std::shared_ptr<logger> log_;

	public:
		std::chrono::time_point<
			std::chrono::high_resolution_clock> start_time;

		/* printing types */
		timer();
		timer(const char* name);
		/* logging  types */
		timer(std::shared_ptr<logger> log__);
		timer(std::shared_ptr<logger> log__, const char* name);

		~timer();

		struct data : logger::data {
			std::chrono::time_point<
				std::chrono::high_resolution_clock> start_time;
			std::chrono::time_point<
				std::chrono::high_resolution_clock> end_time;

			virtual std::ostream&
				append_to_ostream(std::ostream& os) const override;
		};
};
