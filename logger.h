#pragma once

#include <iostream>


class logger {
	public:
		struct data {
			virtual std::ostream& append_to_ostream(std::ostream& os) const = 0;
			friend std::ostream& operator<<(std::ostream& os, const data& dt);
		};

		void log_data(const data &dt);
};
