#ifndef ENUM_MAP_COLLECT_TOYS_H
#define ENUM_MAP_COLLECT_TOYS_H

#include <map>
#include <vector>
#include <utility>
#include <string>
using namespace std;
namespace despot
{

  enum ActionType
{
    detectAction,
    placeAction,
    pickAction,
    navigateAction
	
};



  enum Collect_toysResponseModuleAndTempEnums
  {
	  detect_moduleResponse,
	  place_moduleResponse,
	  place_eSuccess,
	  place_eFailed,
	  pick_moduleResponse,
	  pick_eSuccess,
	  pick_eFailed,
	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,

      default_moduleResponse = 99999, 
	  illegalActionObs = 100000
  };


  struct enum_map_collect_toys
  {
    	  static map<Collect_toysResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<Collect_toysResponseModuleAndTempEnums,std::string> m;
          m[place_eSuccess] = "place_eSuccess";
          m[place_eFailed] = "place_eFailed";
          m[pick_eSuccess] = "pick_eSuccess";
          m[pick_eFailed] = "pick_eFailed";
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[navigate_eFailed] = "navigate_eFailed";
          m[illegalActionObs] = "IllegalActionObs";
          m[default_moduleResponse] = "DefaultObservation";
          return m;
        }

		static map<std::string, Collect_toysResponseModuleAndTempEnums> CreateMapStringToEnum(map<Collect_toysResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, Collect_toysResponseModuleAndTempEnums> m;
		  map<Collect_toysResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[detectAction] = "detect";
          m[placeAction] = "place";
          m[pickAction] = "pick";
          m[navigateAction] = "navigate";

          return m;
        }

    static map<Collect_toysResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, Collect_toysResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
    
  };

} // namespace despot
#endif /* ENUM_MAP_COLLECT_TOYS_H */

 
