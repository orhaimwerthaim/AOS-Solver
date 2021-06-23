#include <string>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "state_var_types.h"

using namespace std;

namespace despot {
	

	tLocation::tLocation()
	{
		actual_location = false;
		discrete_location = eNear_elevator1;
	} 

}// namespace despot
