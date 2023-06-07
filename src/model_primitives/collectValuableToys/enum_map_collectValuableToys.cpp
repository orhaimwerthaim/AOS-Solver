
#include <despot/model_primitives/collectValuableToys/enum_map_collectValuableToys.h> 
using namespace std;
namespace despot
{ 
	map<CollectValuableToysResponseModuleAndTempEnums, std::string> enum_map_collectValuableToys::vecResponseEnumToString;
	map<std::string, CollectValuableToysResponseModuleAndTempEnums> enum_map_collectValuableToys::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_collectValuableToys::vecActionTypeEnumToString;

	void enum_map_collectValuableToys::Init()
	{
		if(enum_map_collectValuableToys::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_collectValuableToys::vecResponseEnumToString = enum_map_collectValuableToys::CreateMapResponseEnumToString();
	 enum_map_collectValuableToys::vecStringToResponseEnum = enum_map_collectValuableToys::CreateMapStringToEnum(enum_map_collectValuableToys::vecResponseEnumToString);
	enum_map_collectValuableToys::vecActionTypeEnumToString = enum_map_collectValuableToys::CreateMapActionTypeEnumToString();
	}
} // namespace despot
