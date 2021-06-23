#include <string>
#include <iostream> 

namespace despot
{
	typedef bool anyValue;
	struct tLocation;
	enum tDiscreteLocation
	{
		eCorridor,
		eOutside_lab211,
		eNear_elevator1,
		eAuditorium,
		eUnknown
	};

	struct tLocation
	{
		tDiscreteLocation discrete_location;
		anyValue actual_location;

		tLocation(); 
	};

 
} // namespace despot
