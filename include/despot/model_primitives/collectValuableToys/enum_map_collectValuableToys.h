#ifndef ENUM_MAP_COLLECTVALUABLETOYS_H
#define ENUM_MAP_COLLECTVALUABLETOYS_H

#include <map>
#include <vector>
#include <utility>
#include <string>
using namespace std;
namespace despot
{

  enum ActionType
{
    navigateAction,
    pickAction,
    placeAction
	
};



  enum CollectValuableToysResponseModuleAndTempEnums
  {
	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,
	  pick_moduleResponse,
	  pick_eSuccess,
	  pick_eFailed,
	  place_moduleResponse,
	  place_eSuccess,
	  place_eFailed,

      default_moduleResponse = 99999, 
	  illegalActionObs = 100000
  };


  struct enum_map_collectValuableToys
  {
    	  static map<CollectValuableToysResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<CollectValuableToysResponseModuleAndTempEnums,std::string> m;
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[navigate_eFailed] = "navigate_eFailed";
          m[pick_eSuccess] = "pick_eSuccess";
          m[pick_eFailed] = "pick_eFailed";
          m[place_eSuccess] = "place_eSuccess";
          m[place_eFailed] = "place_eFailed";
          m[illegalActionObs] = "IllegalActionObs";
          m[default_moduleResponse] = "DefaultObservation";
          return m;
        }

		static map<std::string, CollectValuableToysResponseModuleAndTempEnums> CreateMapStringToEnum(map<CollectValuableToysResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, CollectValuableToysResponseModuleAndTempEnums> m;
		  map<CollectValuableToysResponseModuleAndTempEnums,std::string>::iterator it;
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
          m[pickAction] = "pick";
          m[placeAction] = "place";

          return m;
        }

    static map<CollectValuableToysResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, CollectValuableToysResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
    
  };

} // namespace despot
#endif /* ENUM_MAP_COLLECTVALUABLETOYS_H */

 
