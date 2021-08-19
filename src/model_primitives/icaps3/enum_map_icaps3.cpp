
#include <despot/model_primitives/icaps3/enum_map_icaps3.h> 
using namespace std;
namespace despot
{ 
	map<Icaps3ResponseModuleAndTempEnums, std::string> enum_map_icaps3::vecResponseEnumToString;
	map<std::string, Icaps3ResponseModuleAndTempEnums> enum_map_icaps3::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_icaps3::vecActionTypeEnumToString;

	void enum_map_icaps3::Init()
	{
		if(enum_map_icaps3::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_icaps3::vecResponseEnumToString = enum_map_icaps3::CreateMapResponseEnumToString();
	 enum_map_icaps3::vecStringToResponseEnum = enum_map_icaps3::CreateMapStringToEnum(enum_map_icaps3::vecResponseEnumToString);
	enum_map_icaps3::vecActionTypeEnumToString = enum_map_icaps3::CreateMapActionTypeEnumToString();
	}
} // namespace despot
