#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 
using namespace std;
namespace despot
{
	typedef bool anyValue;





	struct tLocation
	{
		float x;
		float y;
		float z;
		int discrete;
		inline bool operator==(const tLocation& other)const{return (*this).x == other.x && (*this).y == other.y && (*this).z == other.z && (*this).discrete == other.discrete;};
		inline bool operator!=(const tLocation& other)const{return !(*this == other);};
		tLocation(); 
	};



	struct tVisitedLocation
	{
		int discrete;
		bool visited;
		inline bool operator==(const tVisitedLocation& other)const{return (*this).discrete == other.discrete && (*this).visited == other.visited;};
		inline bool operator!=(const tVisitedLocation& other)const{return !(*this == other);};
		tVisitedLocation(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H