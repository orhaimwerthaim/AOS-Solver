#include <despot/model_primitives/icaps/state.h> 
namespace despot {
	

	tLocation::tLocation()
	{
		actual_location = false;
		discrete_location = eNear_elevator1;
	} 



		void IcapsState::SetAnyValueLinks(IcapsState *state)
		{
			state->anyValueUpdateDic["state.cupAccurateLocation"] = &(state->cupAccurateLocation);
			state->anyValueUpdateDic["state.locationOutside_lab211.actual_location"] = &(state->locationOutside_lab211.actual_location);
			state->anyValueUpdateDic["state.locationAuditorium.actual_location"] = &(state->locationAuditorium.actual_location);
			state->anyValueUpdateDic["state.locationCorridor.actual_location"] = &(state->locationCorridor.actual_location);
			state->anyValueUpdateDic["state.locationNear_elevator1.actual_location"] = &(state->locationNear_elevator1.actual_location);
		}
}// namespace despot