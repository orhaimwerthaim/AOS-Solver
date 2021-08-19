#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 

namespace despot
{
	typedef bool anyValue;



	enum tDiscreteLocation
	{
		eOutside_lab211,
		eAuditorium,
		eCorridor,
		eNear_elevator1,
		eUnknown
	};




	struct tLocation
	{
		tDiscreteLocation discrete_location;
		anyValue actual_location;
		tLocation(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H