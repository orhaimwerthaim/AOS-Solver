
#include <despot/model_primitives/icaps/enum_map_icaps.h> 
using namespace std;
namespace despot
{ 
	map<IcapsResponseModuleAndTempEnums, std::string> enum_map_icaps::vecResponseEnumToString;
	map<std::string, IcapsResponseModuleAndTempEnums> enum_map_icaps::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_icaps::vecActionTypeEnumToString;

	void enum_map_icaps::Init()
	{
		if(enum_map_icaps::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_icaps::vecResponseEnumToString = enum_map_icaps::CreateMapResponseEnumToString();
	 enum_map_icaps::vecStringToResponseEnum = enum_map_icaps::CreateMapStringToEnum(enum_map_icaps::vecResponseEnumToString);
	enum_map_icaps::vecActionTypeEnumToString = enum_map_icaps::CreateMapActionTypeEnumToString();
	}
} // namespace despot
