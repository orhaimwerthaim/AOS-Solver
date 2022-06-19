
#include <despot/model_primitives/turtleBotVisitLocations/enum_map_turtleBotVisitLocations.h> 
using namespace std;
namespace despot
{ 
	map<TurtleBotVisitLocationsResponseModuleAndTempEnums, std::string> enum_map_turtleBotVisitLocations::vecResponseEnumToString;
	map<std::string, TurtleBotVisitLocationsResponseModuleAndTempEnums> enum_map_turtleBotVisitLocations::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_turtleBotVisitLocations::vecActionTypeEnumToString;

	void enum_map_turtleBotVisitLocations::Init()
	{
		if(enum_map_turtleBotVisitLocations::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_turtleBotVisitLocations::vecResponseEnumToString = enum_map_turtleBotVisitLocations::CreateMapResponseEnumToString();
	 enum_map_turtleBotVisitLocations::vecStringToResponseEnum = enum_map_turtleBotVisitLocations::CreateMapStringToEnum(enum_map_turtleBotVisitLocations::vecResponseEnumToString);
	enum_map_turtleBotVisitLocations::vecActionTypeEnumToString = enum_map_turtleBotVisitLocations::CreateMapActionTypeEnumToString();
	}
} // namespace despot
