
#include <despot/model_primitives/collect_toys/enum_map_collect_toys.h> 
using namespace std;
namespace despot
{ 
	map<Collect_toysResponseModuleAndTempEnums, std::string> enum_map_collect_toys::vecResponseEnumToString;
	map<std::string, Collect_toysResponseModuleAndTempEnums> enum_map_collect_toys::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_collect_toys::vecActionTypeEnumToString;

	void enum_map_collect_toys::Init()
	{
		if(enum_map_collect_toys::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_collect_toys::vecResponseEnumToString = enum_map_collect_toys::CreateMapResponseEnumToString();
	 enum_map_collect_toys::vecStringToResponseEnum = enum_map_collect_toys::CreateMapStringToEnum(enum_map_collect_toys::vecResponseEnumToString);
	enum_map_collect_toys::vecActionTypeEnumToString = enum_map_collect_toys::CreateMapActionTypeEnumToString();
	}
} // namespace despot
