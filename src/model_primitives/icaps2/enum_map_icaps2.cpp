
#include <despot/model_primitives/icaps2/enum_map_icaps2.h> 
using namespace std;
namespace despot
{ 
	map<Icaps2ResponseModuleAndTempEnums, std::string> enum_map_icaps2::vecResponseEnumToString;
	map<std::string, Icaps2ResponseModuleAndTempEnums> enum_map_icaps2::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_icaps2::vecActionTypeEnumToString;

	void enum_map_icaps2::Init()
	{
		if(enum_map_icaps2::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_icaps2::vecResponseEnumToString = enum_map_icaps2::CreateMapResponseEnumToString();
	 enum_map_icaps2::vecStringToResponseEnum = enum_map_icaps2::CreateMapStringToEnum(enum_map_icaps2::vecResponseEnumToString);
	enum_map_icaps2::vecActionTypeEnumToString = enum_map_icaps2::CreateMapActionTypeEnumToString();
	}
} // namespace despot
