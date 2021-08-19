#ifndef ENUM_MAP_ICAPS2_H
#define ENUM_MAP_ICAPS2_H

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
    placeAction,
    observeAction,
    pickAction,
    navigateAction
	
};



  enum Icaps2ResponseModuleAndTempEnums
  {
	  place_moduleResponse,
	  place_ePlaceActionSuccess,
	  place_eDroppedObject,
	  place_eFailedUnknown,
	  place_enumRealCase,
	  place_success,
	  place_droppedObject,
	  place_unknownFailure,
	  observe_moduleResponse,
	  observe_eObserved,
	  observe_eNotObserved,
	  observe_enumRealCase,
	  observe_observed,
	  observe_notObserved,
	  pick_moduleResponse,
	  pick_res_pick_action_success,
	  pick_res_not_holding,
	  pick_res_broke_the_object,
	  pick_enumRealCase,
	  pick_actual_pick_action_success,
	  pick_actual_not_holding,
	  pick_actual_broke_the_object,
	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,
	  navigate_enumRealCase,
	  navigate_action_success,
	  navigate_failed,

	  illegalActionObs = 100000
  };


  struct enum_map_icaps2
  {
	  static map<Icaps2ResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<Icaps2ResponseModuleAndTempEnums,std::string> m;
          m[place_ePlaceActionSuccess] = "place_ePlaceActionSuccess";
          m[place_eDroppedObject] = "place_eDroppedObject";
          m[place_eFailedUnknown] = "place_eFailedUnknown";
          m[place_success] = "place_success";
          m[place_droppedObject] = "place_droppedObject";
          m[place_unknownFailure] = "place_unknownFailure";
          m[observe_eObserved] = "observe_eObserved";
          m[observe_eNotObserved] = "observe_eNotObserved";
          m[observe_observed] = "observe_observed";
          m[observe_notObserved] = "observe_notObserved";
          m[pick_res_pick_action_success] = "pick_res_pick_action_success";
          m[pick_res_not_holding] = "pick_res_not_holding";
          m[pick_res_broke_the_object] = "pick_res_broke_the_object";
          m[pick_actual_pick_action_success] = "pick_actual_pick_action_success";
          m[pick_actual_not_holding] = "pick_actual_not_holding";
          m[pick_actual_broke_the_object] = "pick_actual_broke_the_object";
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[navigate_eFailed] = "navigate_eFailed";
          m[navigate_action_success] = "navigate_action_success";
          m[navigate_failed] = "navigate_failed";
          m[illegalActionObs] = "IllegalActionObs";
          return m;
        }

		static map<std::string, Icaps2ResponseModuleAndTempEnums> CreateMapStringToEnum(map<Icaps2ResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, Icaps2ResponseModuleAndTempEnums> m;
		  map<Icaps2ResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[placeAction] = "place";
          m[observeAction] = "observe";
          m[pickAction] = "pick";
          m[navigateAction] = "navigate";

          return m;
        }

    static map<Icaps2ResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, Icaps2ResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_ICAPS2_H */

 
