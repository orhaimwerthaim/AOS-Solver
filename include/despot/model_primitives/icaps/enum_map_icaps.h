#ifndef ENUM_MAP_ICAPS_H
#define ENUM_MAP_ICAPS_H

#include <map>

#include "state.h"
#include <vector>
#include <utility>
#include <string>
using namespace std;
namespace despot
{
  enum ActionType
{
    pickAction,
    placeAction,
	observeAction,
	navigateAction
	
};

  enum IcapsResponseModuleAndTempEnums
  {
	  pick_enumRealCase,
	  pick_actual_pick_action_success,
	  pick_actual_not_holding,
	  pick_actual_broke_the_object,

	  pick_moduleResponse,
	  pick_res_pick_action_success,
	  pick_res_not_holding,
	  pick_res_broke_the_object,

	  place_enumRealCase,
	  place_success,
	  place_droppedObject,
	  place_unknownFailure,

	  place_moduleResponse,
	  place_ePlaceActionSuccess,
	  place_eDroppedObject,
	  place_eFailedUnknown,

	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,
	  
	  navigate_enumRealCase,
	  navigate_action_success,
	  navigate_failed,

	  observe_moduleResponse,	
	  observe_eObserved,
	  observe_eNotObserved,

	  observe_enumRealCase,
	  observe_observed,
	  observe_notObserved,
 
	  illegalActionObs = 100000
  };

  struct enum_map_icaps
  {
	  static map<IcapsResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<IcapsResponseModuleAndTempEnums,std::string> m;
          m[pick_res_pick_action_success] = "pick_res_pick_action_success";
          m[pick_res_not_holding] = "pick_res_not_holding";
          m[pick_res_broke_the_object] = "pick_res_broke_the_object";

		  m[place_ePlaceActionSuccess] = "place_ePlaceActionSuccess";
		  m[place_eDroppedObject] = "place_eDroppedObject";
		  m[place_eFailedUnknown] = "place_eFailedUnknown";

		  m[navigate_eSuccess] = "navigate_eSuccess";
		  m[navigate_failed] = "navigate_failed";

		  m[observe_eObserved] = "observe_eObserved";
		  m[observe_eNotObserved] = "observe_eNotObserved";
          return m;
        }

		static map<std::string, IcapsResponseModuleAndTempEnums> CreateMapStringToEnum(map<IcapsResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, IcapsResponseModuleAndTempEnums> m;
		  map<IcapsResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }

	


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[pickAction] = "pick";
          m[navigateAction] = "navigate";
          m[observeAction] = "observe";

		  m[placeAction] = "place";
          return m;
        }
    static map<IcapsResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, IcapsResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_ICAPS_H */

 
