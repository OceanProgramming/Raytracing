#include <cstdlib>

#include "Vector3.hpp"

const double randMax = (double)RAND_MAX;

double getRandom()
{
	return rand() / randMax;
}