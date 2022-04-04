
#include <despot/model_primitives/Bp_with_int_model/enum_map_Bp_with_int_model.h> 
using namespace std;
namespace despot
{ 
	map<Bp_with_int_modelResponseModuleAndTempEnums, std::string> enum_map_Bp_with_int_model::vecResponseEnumToString;
	map<std::string, Bp_with_int_modelResponseModuleAndTempEnums> enum_map_Bp_with_int_model::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_Bp_with_int_model::vecActionTypeEnumToString;

	void enum_map_Bp_with_int_model::Init()
	{
		if(enum_map_Bp_with_int_model::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_Bp_with_int_model::vecResponseEnumToString = enum_map_Bp_with_int_model::CreateMapResponseEnumToString();
	 enum_map_Bp_with_int_model::vecStringToResponseEnum = enum_map_Bp_with_int_model::CreateMapStringToEnum(enum_map_Bp_with_int_model::vecResponseEnumToString);
	enum_map_Bp_with_int_model::vecActionTypeEnumToString = enum_map_Bp_with_int_model::CreateMapActionTypeEnumToString();
	}
} // namespace despot
