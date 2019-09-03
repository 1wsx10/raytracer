#pragma once

#include <chrono>
#include <stdio.h>

using namespace std::chrono;

class timer {
	public:
		time_point<high_resolution_clock> start_time;

		timer() : start_time(high_resolution_clock::now()) {};

		~timer() {
			time_point<high_resolution_clock> end_time = high_resolution_clock::now();

			std::chrono::duration<double> seconds = end_time - start_time;
			std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(seconds);

			printf("%fs\n%dus\n", seconds.count(), static_cast<int>(us.count()));
		}
};
