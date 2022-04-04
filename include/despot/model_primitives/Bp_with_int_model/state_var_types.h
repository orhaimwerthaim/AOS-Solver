#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 

namespace despot
{
	typedef bool anyValue;



	enum tPushType
	{
		SingleAgentPush,
		JointPush
	};


	enum tDirection
	{
		Up,
		Down,
		Left,
		Right,
		None
	};




	struct tLocation
	{
		int x;
		int y;
		inline bool operator==(const tLocation& other)const{return (*this).x == other.x && (*this).y == other.y;};
		inline bool operator!=(const tLocation& other)const{return !(*this == other);};
		tLocation(); 
	};



	struct tDirectionParam
	{
		tDirection direction;
		inline bool operator==(const tDirectionParam& other)const{return (*this).direction == other.direction;};
		inline bool operator!=(const tDirectionParam& other)const{return !(*this == other);};
		tDirectionParam(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H