#pragma once

#include <chrono>
#include <cstdio>
#include <string>
#include <iostream>
#include <memory>

using namespace std::chrono;

class logger {
	public:
		struct data {
			virtual std::ostream& append_to_ostream(std::ostream& os) const = 0;
			friend std::ostream& operator<<(std::ostream& os, const data& dt);
		};

		void log_data(const data &dt);
};

std::ostream& operator<<(std::ostream& os, const logger::data& dt) {
	return dt.append_to_ostream(os);
}

class timer {
		std::string name;
		enum TYPE {
			PRINTING,
			LOGGING
		} type;
		std::shared_ptr<logger> log_;

	public:
		time_point<high_resolution_clock> start_time;

		timer(std::string name) : name(name), type(PRINTING), log_(nullptr), start_time(high_resolution_clock::now()) {};
		timer(std::shared_ptr<logger> log__) : name(""), type(LOGGING), log_(log__), start_time(high_resolution_clock::now()) {};

		~timer() {
			time_point<high_resolution_clock> end_time = high_resolution_clock::now();

			std::chrono::duration<double> seconds;
			std::chrono::microseconds us;
			data dt;

			switch(type) {

				case PRINTING:
					seconds = end_time - start_time;
					us = std::chrono::duration_cast<std::chrono::microseconds>(seconds);

					printf("%s %10dus\n", name.c_str(), static_cast<int>(us.count()));
					break;

				case LOGGING:
					dt.start_time = start_time;
					dt.end_time = end_time;
					log_->log_data(dt);
					break;
			}
		}

		struct data : logger::data {
			std::chrono::time_point<high_resolution_clock> start_time;
			std::chrono::time_point<high_resolution_clock> end_time;

			virtual std::ostream& append_to_ostream(std::ostream& os) const override {
				std::chrono::duration<double> duration_seconds = end_time - start_time;
				std::chrono::microseconds duration_us = std::chrono::duration_cast<std::chrono::microseconds>(duration_seconds);

				// TODO better than this
				os << duration_us.count();
				return os;
			}
		};
};
