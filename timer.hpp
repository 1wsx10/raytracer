#pragma once

#include <chrono>
#include <cstdio>
#include <string>
#include <iostream>
#include <memory>

#include "logger.h"

class timer {
	static unsigned int count;

	std::string name;
	enum TYPE {
		PRINTING,
		LOGGING
	} type;
	std::shared_ptr<logger> log_;

	public:
		std::chrono::time_point<
			std::chrono::high_resolution_clock> start_time;

		timer();
		timer(std::string name);
		timer(std::shared_ptr<logger> log__);
		timer(std::shared_ptr<logger> log__, std::string name);

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
