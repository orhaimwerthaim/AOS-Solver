#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 
using namespace std;
namespace despot
{
	typedef bool anyValue;



	enum eLocation
	{
		L0,
		L1,
		L2,
		L3,
		LChild,
		LArm
	};




	struct tToy
	{
		int location;
		float reward;
		string type;
		inline bool operator==(const tToy& other)const{return (*this).location == other.location && (*this).reward == other.reward && (*this).type == other.type;};
		inline bool operator!=(const tToy& other)const{return !(*this == other);};
		tToy(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H