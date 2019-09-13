#include "timer.hpp"

using namespace std::chrono;

timer::timer() : timer("timer") {};

timer::timer(const char* name) :
	name(name),
	type(timer::TYPE::PRINTING),
	log_(nullptr),
	start_time(high_resolution_clock::now()) {};

timer::timer(std::shared_ptr<logger> log__) : timer(log__, nullptr) {}

timer::timer(std::shared_ptr<logger> log__, const char* name) :
	name(name),
	type(timer::TYPE::LOGGING),
	log_(log__),
	start_time(high_resolution_clock::now()) {};


timer::~timer()  {
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();

	std::chrono::duration<double> seconds;
	std::chrono::microseconds us;
	data dt;

	switch(type) {

		case PRINTING:
			seconds = end_time - start_time;
			us = std::chrono::duration_cast<std::chrono::microseconds>(seconds);

			if(name)
				printf("%s: %10dus\n", name, static_cast<int>(us.count()));
			break;

		case LOGGING:
			dt.start_time = start_time;
			dt.end_time = end_time;
			log_->log_data(dt);
			break;
	}
}

std::ostream& timer::data::append_to_ostream(std::ostream& os) const {
	std::chrono::duration<double> duration_seconds = end_time - start_time;
	std::chrono::microseconds duration_us = std::chrono::duration_cast<std::chrono::microseconds>(duration_seconds);

	os << duration_us.count();
	return os;
}
