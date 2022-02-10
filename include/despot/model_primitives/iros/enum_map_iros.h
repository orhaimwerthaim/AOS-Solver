#ifndef ENUM_MAP_IROS_H
#define ENUM_MAP_IROS_H

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
    serve_can_to_personAction,
    navigateAction,
    observe_holding_canAction,
    observe_arm_outstretchedAction,
    basic_pickAction
	
};



  enum IrosResponseModuleAndTempEnums
  {
	  serve_can_to_person_moduleResponse,
	  serve_can_to_person_eDelivered,
	  serve_can_to_person_eNotDelivered,
	  serve_can_to_person_enumRealCase,
	  serve_can_to_person_action_success,
	  serve_can_to_person_failed,
	  navigate_moduleResponse,
	  navigate_eSuccess,
	  navigate_eFailed,
	  navigate_enumRealCase,
	  navigate_action_success,
	  navigate_failed,
	  observe_holding_can_moduleResponse,
	  observe_holding_can_eHoldingCan,
	  observe_holding_can_eNotHoldingCan,
	  observe_arm_outstretched_moduleResponse,
	  observe_arm_outstretched_eArmOutstretched,
	  observe_arm_outstretched_eArmNotOutstretched,
	  basic_pick_moduleResponse,
	  basic_pick_res_pick_action_success,
	  basic_pick_res_pick_action_failed,
	  basic_pick_enumRealCase,
	  basic_pick_actual_pick_action_success,
	  basic_pick_actual_arm_outstretched,
	  basic_pick_actual_not_holding,
	  basic_pick_actual_dropped_the_object,

	  illegalActionObs = 100000
  };


  struct enum_map_iros
  {
	  static map<IrosResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<IrosResponseModuleAndTempEnums,std::string> m;
          m[serve_can_to_person_eDelivered] = "serve_can_to_person_eDelivered";
          m[serve_can_to_person_eNotDelivered] = "serve_can_to_person_eNotDelivered";
          m[serve_can_to_person_action_success] = "serve_can_to_person_action_success";
          m[serve_can_to_person_failed] = "serve_can_to_person_failed";
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[navigate_eFailed] = "navigate_eFailed";
          m[navigate_action_success] = "navigate_action_success";
          m[navigate_failed] = "navigate_failed";
          m[observe_holding_can_eHoldingCan] = "observe_holding_can_eHoldingCan";
          m[observe_holding_can_eNotHoldingCan] = "observe_holding_can_eNotHoldingCan";
          m[observe_arm_outstretched_eArmOutstretched] = "observe_arm_outstretched_eArmOutstretched";
          m[observe_arm_outstretched_eArmNotOutstretched] = "observe_arm_outstretched_eArmNotOutstretched";
          m[basic_pick_res_pick_action_success] = "basic_pick_res_pick_action_success";
          m[basic_pick_res_pick_action_failed] = "basic_pick_res_pick_action_failed";
          m[basic_pick_actual_pick_action_success] = "basic_pick_actual_pick_action_success";
          m[basic_pick_actual_arm_outstretched] = "basic_pick_actual_arm_outstretched";
          m[basic_pick_actual_not_holding] = "basic_pick_actual_not_holding";
          m[basic_pick_actual_dropped_the_object] = "basic_pick_actual_dropped_the_object";
          m[illegalActionObs] = "IllegalActionObs";
          return m;
        }

		static map<std::string, IrosResponseModuleAndTempEnums> CreateMapStringToEnum(map<IrosResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, IrosResponseModuleAndTempEnums> m;
		  map<IrosResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[serve_can_to_personAction] = "serve_can_to_person";
          m[navigateAction] = "navigate";
          m[observe_holding_canAction] = "observe_holding_can";
          m[observe_arm_outstretchedAction] = "observe_arm_outstretched";
          m[basic_pickAction] = "basic_pick";

          return m;
        }

    static map<IrosResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, IrosResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_IROS_H */

 
