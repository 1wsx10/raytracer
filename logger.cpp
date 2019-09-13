#include "logger.h"


logger::logger(std::ofstream out) : out_stream(std::move(out)) {};
logger::logger() : out_stream(nullptr) {};

logger::logger(std::string name) : logger(std::ofstream(name, std::ios::out)) {};

std::ostream& operator<<(std::ostream& os, const logger::data& dt) {
	return dt.append_to_ostream(os);
}

void logger::log_data(const data &dt) {
	if(out_stream)
		out_stream << dt << std::endl;
	else
		std::cout << dt << std::endl;
}


