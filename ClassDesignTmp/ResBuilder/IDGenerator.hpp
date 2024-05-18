#include <ctime>

class IDGenerator
{
	public:
    static int generateID();

	private:
			static int count;
};
