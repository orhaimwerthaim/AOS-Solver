#include <string>
#include <iostream> 
#ifndef STATE_VAR_TYPES_H
#define STATE_VAR_TYPES_H
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
#endif //STATE_VAR_TYPES_H