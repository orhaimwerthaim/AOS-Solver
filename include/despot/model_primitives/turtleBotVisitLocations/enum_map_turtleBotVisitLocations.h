#ifndef ENUM_MAP_TURTLEBOTVISITLOCATIONS_H
#define ENUM_MAP_TURTLEBOTVISITLOCATIONS_H

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



  enum TurtleBotVisitLocationsResponseModuleAndTempEnums
  {
	  navigate_moduleResponse,
	  navigate_eSuccess,

	  illegalActionObs = 100000
  };


  struct enum_map_turtleBotVisitLocations
  {
	  static map<TurtleBotVisitLocationsResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<TurtleBotVisitLocationsResponseModuleAndTempEnums,std::string> m;
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[illegalActionObs] = "IllegalActionObs";
          return m;
        }

		static map<std::string, TurtleBotVisitLocationsResponseModuleAndTempEnums> CreateMapStringToEnum(map<TurtleBotVisitLocationsResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, TurtleBotVisitLocationsResponseModuleAndTempEnums> m;
		  map<TurtleBotVisitLocationsResponseModuleAndTempEnums,std::string>::iterator it;
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

    static map<TurtleBotVisitLocationsResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, TurtleBotVisitLocationsResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_TURTLEBOTVISITLOCATIONS_H */

 
