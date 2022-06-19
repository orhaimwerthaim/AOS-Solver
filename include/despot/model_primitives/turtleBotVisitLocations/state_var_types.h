#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 

namespace despot
{
	typedef bool anyValue;



	enum tDiscreteLocation
	{
		eL1,
		eL2,
		eL3,
		eL4,
		eL5,
		eL6,
		eL7,
		eL8
	};




	struct tLocation
	{
		float x;
		float y;
		float z;
		tDiscreteLocation desc;
		inline bool operator==(const tLocation& other)const{return (*this).x == other.x && (*this).y == other.y && (*this).z == other.z && (*this).desc == other.desc;};
		inline bool operator!=(const tLocation& other)const{return !(*this == other);};
		tLocation(); 
	};



	struct tVisitedLocation
	{
		bool visited;
		tDiscreteLocation desc;
		inline bool operator==(const tVisitedLocation& other)const{return (*this).visited == other.visited && (*this).desc == other.desc;};
		inline bool operator!=(const tVisitedLocation& other)const{return !(*this == other);};
		tVisitedLocation(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H