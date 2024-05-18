#include "IDGenerator.hpp"

int IDGenerator::count = 0;

int IDGenerator::generateID()
{
		// ID = current timestamp * 1000 + count from 0 to 999

		if (count == 1000)
			count = 0;

		time_t current_time = time(NULL);
		int id = static_cast<int>(current_time) * 1000 + count++;

		return (id);
}
