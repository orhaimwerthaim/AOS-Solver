#ifndef VAR_TYPES_H
#define VAR_TYPES_H

#include <string>
#include <iostream> 
using namespace std;
namespace despot
{
	typedef bool anyValue;





	struct tToy
	{
		int location;
		string type;
		float reward;
		inline bool operator==(const tToy& other)const{return (*this).location == other.location && (*this).type == other.type && (*this).reward == other.reward;};
		inline bool operator!=(const tToy& other)const{return !(*this == other);};
		tToy(); 
	};



 
} // namespace despot
#endif //VAR_TYPES_H