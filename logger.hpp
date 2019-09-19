#pragma once

#include <iostream>
#include <fstream>


class logger {
	public:
		struct data {
			virtual std::ostream& append_to_ostream(std::ostream& os) const = 0;
			friend std::ostream& operator<<(std::ostream& os, const data& dt);
		};

		logger(std::ofstream of);
		logger();
		logger(std::string of_name);

		void log_data(const data &dt);

	private:
		std::ofstream out_stream;
};
