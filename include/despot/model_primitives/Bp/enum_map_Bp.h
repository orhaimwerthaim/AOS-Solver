#ifndef ENUM_MAP_BP_H
#define ENUM_MAP_BP_H

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



  enum BpResponseModuleAndTempEnums
  {
	  push_moduleResponse,
	  push_eSuccess,
	  navigate_moduleResponse,
	  navigate_eSuccess,

	  illegalActionObs = 100000
  };


  struct enum_map_Bp
  {
	  static map<BpResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<BpResponseModuleAndTempEnums,std::string> m;
          m[push_eSuccess] = "push_eSuccess";
          m[navigate_eSuccess] = "navigate_eSuccess";
          m[illegalActionObs] = "IllegalActionObs";
          return m;
        }

		static map<std::string, BpResponseModuleAndTempEnums> CreateMapStringToEnum(map<BpResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, BpResponseModuleAndTempEnums> m;
		  map<BpResponseModuleAndTempEnums,std::string>::iterator it;
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

    static map<BpResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, BpResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
  };

} // namespace despot
#endif /* ENUM_MAP_BP_H */

 
