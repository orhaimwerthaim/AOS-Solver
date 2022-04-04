#ifndef ENUM_MAP_BP_WITH_INT_MODEL_H
#define ENUM_MAP_BP_WITH_INT_MODEL_H

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
    pushAction,
    navigateAction
	
};



  enum Bp_with_int_modelResponseModuleAndTempEnums
  {
	  push_moduleResponse,
	  push_eSuccess,
	  navigate_moduleResponse,
	  navigate_eSuccess,

	  illegalActionObs = 100000
  };


  struct enum_map_Bp_with_int_model
  {
	  static map<Bp_with_int_modelResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<Bp_with_int_modelResponseModuleAndTempEnums,std::string> m;
          m[push_eSuccess] = "push_eSuccess";
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[illegalActionObs] = "IllegalActionObs";
          return m;
        }

		static map<std::string, Bp_with_int_modelResponseModuleAndTempEnums> CreateMapStringToEnum(map<Bp_with_int_modelResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, Bp_with_int_modelResponseModuleAndTempEnums> m;
		  map<Bp_with_int_modelResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[pushAction] = "push";
          m[navigateAction] = "navigate";

          return m;
        }

    static map<Bp_with_int_modelResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, Bp_with_int_modelResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_BP_WITH_INT_MODEL_H */

 
