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


	enum tCell
	{
		L00,
		L10,
		L01,
		L11,
		OUT
	};




	struct tDirectionParam
	{
		tDirection direction;
		tDirectionParam(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H