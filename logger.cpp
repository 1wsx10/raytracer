#include "logger.h"


std::ostream& operator<<(std::ostream& os, const logger::data& dt) {
	return dt.append_to_ostream(os);
}

void logger::log_data(const data &dt) {
	std::cout << dt << std::endl;
}


