
#include <despot/model_primitives/example/enum_map_example.h> 
using namespace std;
namespace despot
{ 
	map<ExampleResponseModuleAndTempEnums, std::string> enum_map_example::vecResponseEnumToString;
	map<std::string, ExampleResponseModuleAndTempEnums> enum_map_example::vecStringToResponseEnum ;
	map<ActionType,std::string> enum_map_example::vecActionTypeEnumToString;

	void enum_map_example::Init()
	{
		if(enum_map_example::vecResponseEnumToString.size() > 0)
			return; 

		enum_map_example::vecResponseEnumToString = enum_map_example::CreateMapResponseEnumToString();
	 enum_map_example::vecStringToResponseEnum = enum_map_example::CreateMapStringToEnum(enum_map_example::vecResponseEnumToString);
	enum_map_example::vecActionTypeEnumToString = enum_map_example::CreateMapActionTypeEnumToString();
	}
} // namespace despot
