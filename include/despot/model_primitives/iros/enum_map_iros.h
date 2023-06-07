#ifndef ENUM_MAP_IROS_H
#define ENUM_MAP_IROS_H

#include <map>
#include <vector>
#include <utility>
#include <string>
using namespace std;
namespace despot
{

  enum ActionType
{
    detect_board_stateAction,
    draw_in_cellAction
	
};



  enum IrosResponseModuleAndTempEnums
  {
	  detect_board_state_moduleResponse,
	  draw_in_cell_moduleResponse,
	  draw_in_cell_res_success,

      default_moduleResponse = 99999, 
	  illegalActionObs = 100000
  };


  struct enum_map_iros
  {
    	  static map<IrosResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<IrosResponseModuleAndTempEnums,std::string> m;
          m[draw_in_cell_res_success] = "draw_in_cell_res_success";
          m[illegalActionObs] = "IllegalActionObs";
          m[default_moduleResponse] = "DefaultObservation";
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
          m[detect_board_stateAction] = "detect_board_state";
          m[draw_in_cellAction] = "draw_in_cell";

          return m;
        }

    static map<IrosResponseModuleAndTempEnums,std::string> vecResponseEnumToString;
	static map<std::string, IrosResponseModuleAndTempEnums> vecStringToResponseEnum;
	static map<ActionType,std::string> vecActionTypeEnumToString;
	static void Init();
    
  };

} // namespace despot
#endif /* ENUM_MAP_IROS_H */

 
