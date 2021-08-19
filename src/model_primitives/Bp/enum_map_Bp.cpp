
#include <despot/model_primitives/Bp/enum_map_Bp.h> 
using namespace std;
namespace despot
{ 
	map<BpResponseModuleAndTempEnums, std::string> enum_map_Bp::vecResponseEnumToString;
	map<std::string, BpResponseModuleAndTempEnums> enum_map_Bp::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_Bp::vecActionTypeEnumToString;

	void enum_map_Bp::Init()
	{
		if(enum_map_Bp::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_Bp::vecResponseEnumToString = enum_map_Bp::CreateMapResponseEnumToString();
	 enum_map_Bp::vecStringToResponseEnum = enum_map_Bp::CreateMapStringToEnum(enum_map_Bp::vecResponseEnumToString);
	enum_map_Bp::vecActionTypeEnumToString = enum_map_Bp::CreateMapActionTypeEnumToString();
	}
} // namespace despot
