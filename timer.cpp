#include "timer.hpp"

timer::timer(std::string name)
	: name(name),
	type(timer::TYPE::PRINTING),
	log_(nullptr),
	start_time(high_resolution_clock::now()) {};

timer::timer(std::shared_ptr<logger> log__)
	: name(""),
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

			printf("%s %10dus\n", name.c_str(), static_cast<int>(us.count()));
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

	// TODO better than this
	os << duration_us.count();
	return os;
}
