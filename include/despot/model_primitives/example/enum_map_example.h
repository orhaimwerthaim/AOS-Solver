#ifndef ENUM_MAP_EXAMPLE_H
#define ENUM_MAP_EXAMPLE_H

#include <map>
#include <vector>
#include <utility>
#include <string>
using namespace std;
namespace despot
{

  enum ActionType
{
    navigateAction
	
};



  enum ExampleResponseModuleAndTempEnums
  {
	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,

      default_moduleResponse = 99999, 
	  illegalActionObs = 100000
  };


  struct enum_map_example
  {
    	  static map<ExampleResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<ExampleResponseModuleAndTempEnums,std::string> m;
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[navigate_eFailed] = "navigate_eFailed";
          m[illegalActionObs] = "IllegalActionObs";
          m[default_moduleResponse] = "DefaultObservation";
          return m;
        }

		static map<std::string, ExampleResponseModuleAndTempEnums> CreateMapStringToEnum(map<ExampleResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, ExampleResponseModuleAndTempEnums> m;
		  map<ExampleResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[navigateAction] = "navigate";

          return m;
        }

    static map<ExampleResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, ExampleResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
    
  };

} // namespace despot
#endif /* ENUM_MAP_EXAMPLE_H */

 
