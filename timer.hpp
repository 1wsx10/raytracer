#pragma once

#include <chrono>
#include <stdio.h>
#include <string>

using namespace std::chrono;

class timer {
		std::string name;
	public:
		time_point<high_resolution_clock> start_time;

		timer(std::string name) : name(name), start_time(high_resolution_clock::now()) {};

		~timer() {
			time_point<high_resolution_clock> end_time = high_resolution_clock::now();

			std::chrono::duration<double> seconds = end_time - start_time;
			std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(seconds);

			printf("%s %10dus\n", name.c_str(), static_cast<int>(us.count()));
		}
};
