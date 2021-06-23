#include "icaps.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>

using namespace std;

namespace despot {

enum pick_enumRealCase
	{
		actual_pick_action_success,
		actual_not_holding,
		actual_broke_the_object
	};

enum enumIllegalActionObs
{
	illegalActionObs = 100
};
enum pick_enumResponse
{
	res_pick_action_success,
	res_not_holding,
	res_broke_the_object
};

enum place_enumRealCase
{
	success,
	droppedObject,
	unknownFailure
};

enum place_enumResponse
{
	ePlaceActionSuccess,
	eDroppedObject,
	eFailedUnknown
};

enum navigate_enumResponse
{
	eSuccess,
	eFailed,
};

enum navigate_enumRealCase
{
	action_success,
	failed
};

enum observe_enumResponse
{
	eObserved,
	eNotObserved,
};

enum observe_enumRealCase
{
	observed,
	notObserved
};

/* ==============================================================================
 *IcapsBelief class
 * ==============================================================================*/
int IcapsBelief::num_particles = 5000;


IcapsBelief::IcapsBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	icaps_(static_cast<const Icaps*>(model)) {
}
 
std::string Prints::PrintLocation(tDiscreteLocation loc)
{
	switch (loc)
	{
	case eCorridor:
		return "eCorridor";
	case eOutside_lab211:
		return "eOutside_lab211";
	case eNear_elevator1:
		return "eNear_elevator1";
	case eAuditorium:
		return "eAuditorium";
	case eUnknown:
		return "eUnknown";
	}
}
	std::string Prints::PrintActionDescription(ActionDescription* act)
	{
		stringstream ss;
		ss << "ID:" << act->actionId;
		ss << "," << PrintActionType(act->actionType);
		
		if(act->actionType == navigateAction)
		{
			NavigateActionDescription *navA = static_cast<NavigateActionDescription *>(act);
			ss << "," << Prints::PrintLocation((tDiscreteLocation)navA->oDesiredLocation.discrete_location);
		}
		return ss.str();
	}
std::string Prints::PrintObs(int action, int obs)
{
	switch (ActionManager::actions[action]->actionType)
	{
	case pickAction:
		switch ((pick_enumResponse)obs)
		{
		case res_pick_action_success:
			return "res_pick_action_success";
		case res_not_holding:
			return "res_not_holding";
		case res_broke_the_object:
			return "res_broke_the_object"; 
		case illegalActionObs:
			return "illegalActionObs";
		default:
			return "Error obs!!!";
		}
	case placeAction:
		switch ((place_enumResponse)obs)
		{
		case ePlaceActionSuccess:
			return "ePlaceActionSuccess";
		case eDroppedObject:
			return "eDroppedObject";
		case eFailedUnknown:
			return "eFailedUnknown"; 
		case illegalActionObs:
			return "illegalActionObs";
		default:
			return "Error obs!!!";
		} 
	case navigateAction:
		switch((navigate_enumResponse)obs)
		{
		case eSuccess:
			return "eSuccess";
		case eFailed:
			return "eFailed";
		case eFailedUnknown:
			return "eFailedUnknown"; 
		case illegalActionObs:
			return "illegalActionObs";
		default:
			return "Error obs!!!";
		} 
	case observeAction:
		switch((observe_enumResponse)obs)
		{
		case eObserved:
			return "eObserved";
		case eFailed:
			return "eNotObserved";  
		case illegalActionObs:
			return "illegalActionObs";
		default:
			return "Error obs(unknown action observation)!!!";
		} 
	default:
		return "Error obs(unknown action)!!!";
	}
     
}
	std::string Prints::PrintState(IcapsState farstate)
	{
		stringstream ss;
		ss << "STATE: robotLocation:" << Prints::PrintLocation(farstate.robotGenerallocation);
		ss << "|Hand Empty:" << ((farstate.handEmpty) ? "True" : "False");
		ss <<"|CupLocation:" << Prints::PrintLocation(farstate.cupDiscreteGeneralLocation) << endl;
		return ss.str();
	} 
	
 	std::string Icaps::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

std::string Prints::PrintActionType(ActionType actType)
{	
	switch (actType)
		{
		case pickAction:
			return "pickAction";
		case placeAction:
			return "placeAction";
		case observeAction:
			return "observeAction";
		case navigateAction:
			return "navigateAction"; 
		}
}
void IcapsBelief::Update(int actionId, OBS_TYPE obs) {
	history_.Add(actionId, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = icaps_->Copy(particles_[cur]);
		bool terminal = icaps_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);

		if (!terminal && o == obs)
			//|| icaps_->LocalMove(*particle, history_, obs)) 
			{
			updated.push_back(particle);
		} else {
			icaps_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		icaps_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
}

/* ==============================================================================
 * Icaps class
 * ==============================================================================*/

Icaps::Icaps(){
	
}

int Icaps::NumActions() const {
	return ActionManager::actions.size();
}

double Icaps::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Icaps::generator;

//generated for line: sampleDiscrete(tDiscreteLocation,{0, 0.4,0,0.6});
std::discrete_distribution<> Icaps::discrete_dist1{0.6, 0.4,0,0};
std::discrete_distribution<> Icaps::pick_discrete_dist1{0.8, 0.0,0.0};
std::discrete_distribution<> Icaps::place_discrete_dist1{0.9, 0.0, 0.0};
std::discrete_distribution<> Icaps::navigate_discrete_dist1{0.9, 0.0};

std::normal_distribution<double> Icaps::normal_dist1(40000,10000); 
std::normal_distribution<double> Icaps::place_normal_dist1(30000,15000); 


State* Icaps::CreateStartState(string tyep) const {
	IcapsState* startState = memory_pool_.Allocate();
	IcapsState& state = *startState;
	startState->cupDiscreteGeneralLocation = eCorridor;
	startState->handEmpty = true;

	state.locationOutside_lab211 = tLocation();
	state.locationAuditorium = tLocation();
	state.locationCorridor = tLocation();
	state.locationNear_elevator1 = tLocation();

	state.locationOutside_lab211.discrete_location = eOutside_lab211;
	state.locationOutside_lab211.actual_location = true;
	state.locationAuditorium.discrete_location = eAuditorium; state.locationAuditorium.actual_location = true;
	state.locationNear_elevator1.discrete_location = eNear_elevator1; state.locationNear_elevator1.actual_location = true;
	state.locationCorridor.discrete_location = eCorridor; state.locationCorridor.actual_location = true;

	startState->tDiscreteLocationObjects.push_back(eCorridor);
	startState->tDiscreteLocationObjects.push_back(eOutside_lab211);
	startState->tDiscreteLocationObjects.push_back(eNear_elevator1);
	startState->tDiscreteLocationObjects.push_back(eAuditorium);

	startState->tLocationObjects.push_back(state.locationOutside_lab211);
	startState->tLocationObjects.push_back(state.locationAuditorium);
	startState->tLocationObjects.push_back(state.locationNear_elevator1);
	startState->tLocationObjects.push_back(state.locationCorridor);

	state.robotGenerallocation = state.locationNear_elevator1.discrete_location;
	state.cupAccurateLocation = false;
	//generated from environment file line: state.cupDiscreteGeneralLocation = AOS.SampleDiscrete(tDiscreteLocation,{0.6, 0.4,0,0});
	state.cupDiscreteGeneralLocation = state.tDiscreteLocationObjects[discrete_dist1(generator)];
	if(ActionManager::actions.size()==0)
	{
		ActionManager(const_cast <IcapsState*> (startState));
	}
	return startState;
}



Belief* Icaps::InitialBelief(const State* start, string type) const {
	int N = IcapsBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}

	return new IcapsBelief(particles, this);
}
 

 

POMCPPrior* Icaps::CreatePOMCPPrior(string name) const { 
		return new UniformPOMCPPrior(this);
}

void Icaps::PrintState(const State& state, ostream& ostr) const {
	const IcapsState& farstate = static_cast<const IcapsState&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
	// ostr << "called PrintState(): robotLocation:" << Prints::PrintLocation(farstate.robotGenerallocation) << endl;
	// ostr << "|Hand Empty:" << ((farstate.handEmpty) ? "True" : "False") << endl;
	// ostr <<"|CupLocation:" << Prints::PrintLocation(farstate.cupDiscreteGeneralLocation) << endl;
}

void Icaps::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const IcapsState& farstate = static_cast<const IcapsState&>(state);
	
	ostr << observation <<endl;
}

void Icaps::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Icaps::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Icaps::Allocate(int state_id, double weight) const {
	IcapsState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Icaps::Copy(const State* particle) const {
	IcapsState* state = memory_pool_.Allocate();
	*state = *static_cast<const IcapsState*>(particle);
	state->SetAllocated();
	return state;
}

void Icaps::Free(State* particle) const {
	memory_pool_.Free(static_cast<IcapsState*>(particle));
}

int Icaps::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

bool Icaps::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
	bool isNextStateFinal = false;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	
	IcapsState &state__ = static_cast<IcapsState &>(s_state__);
	 logd << "[Icaps::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	if (!meetPrecondition)
	{
		__moduleExecutionTime = 0;
		observation = illegalActionObs;
		return false;
	}

	State *s_state = Copy(&s_state__);
	IcapsState &state = static_cast<IcapsState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, reward, __moduleExecutionTime);

	State *s_state_ = Copy(&s_state__);
	IcapsState &state_ = static_cast<IcapsState &>(*s_state_);

	ModuleDynamicModel(state, state_, state__, rand_num, actionId, reward,
					   observation, __moduleExecutionTime);
	//IcapsState& afterExtrinsicChanges = static_cast<IcapsState&>(Copy(&s_state__));
	
	Free(s_state);
	Free(s_state_);
	bool finalState = ProcessSpecialStates(state__, reward);
	return finalState;
}

void Icaps::CheckPreconditions(const IcapsState& state, double &reward, bool &meetPrecondition, int actionId) const
	{
		meetPrecondition = false;
		//Pick PLP Line: "global_variable_conditionCode": "AOS.IsInitialized(state.cupAccurateLocation) && hand_empty == true && state.robotGenerallocation == state.cupDiscreteGeneralLocation",
		if(ActionManager::actions[actionId]->actionType == pickAction)
		{
			if(state.cupAccurateLocation == true && state.handEmpty == true && state.robotGenerallocation == state.cupDiscreteGeneralLocation)
			{
				meetPrecondition = true;
				 
			}
			 
		}

		//Place PLP Line: "dynamicModelPreconditions": "state.handEmpty == false && (state.robotGenerallocatio == eAuditorium || state.robotGenerallocatio == eOutside_lab211 || state.robotGenerallocatio == eCorridor)",
		if(ActionManager::actions[actionId]->actionType == placeAction)
		{
			if(state.handEmpty == false && (state.robotGenerallocation == eAuditorium || state.robotGenerallocation == eOutside_lab211 || state.robotGenerallocation == eCorridor))
			{
				meetPrecondition = true;
				 
			} 
		}

		if(ActionManager::actions[actionId]->actionType == navigateAction)
		{ 
			meetPrecondition = true;
			 
		}

		if(ActionManager::actions[actionId]->actionType == observeAction)
		{
			meetPrecondition = true;
			 
		}

		if(!meetPrecondition)
			{
				reward += -400;
			}
	}

void Icaps::SampleModuleExecutionTime(const IcapsState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
	{
		if(ActionManager::actions[actionId]->actionType == pickAction)
		{
			__moduleExecutionTime = Icaps::normal_dist1(Icaps::generator);
		}
		if(ActionManager::actions[actionId]->actionType == placeAction)
		{
			__moduleExecutionTime = Icaps::place_normal_dist1(Icaps::generator);
		}
	}

void Icaps::ExtrinsicChangesDynamicModel(const IcapsState& state, IcapsState& state_, double rand_num, int actionId, double& reward,
		const int &__moduleExecutionTime)  const
	{	
	}

	

	void Icaps::ModuleDynamicModel(const IcapsState &state, const IcapsState &state_, IcapsState &state__, double rand_num, int actionId, double &__reward,
								   OBS_TYPE &observation, const int &__moduleExecutionTime) const
	{
		observation = -1;
		int startObs = observation;
		OBS_TYPE &__moduleResponse = observation;

		if (ActionManager::actions[actionId]->actionType == pickAction)
		{
			//layer name: 'realCase'
			pick_enumRealCase realCase = (pick_enumRealCase)Icaps::pick_discrete_dist1(Icaps::generator);
			
			//layer name: 'handEmpty'
			//TODO: uncomment and delete the next line
			state__.handEmpty = (realCase == actual_pick_action_success) ? false: true;
			
			//layer name: 'cupDiscreteGeneralLocation'
			if(realCase == actual_pick_action_success || realCase == actual_broke_the_object)
			{
				state__.cupDiscreteGeneralLocation = eUnknown; state__.cupAccurateLocation = false;
			}
			
			//layer name: '__moduleResponse'
			if (realCase == actual_not_holding) 
			{
				if (AOSUtils::Bernoulli(0.9))
					__moduleResponse= res_not_holding; 
				else __moduleResponse = res_pick_action_success;
			}
			if (realCase == actual_pick_action_success) 
			{
				if (AOSUtils::Bernoulli(0.9)) 
					__moduleResponse = res_pick_action_success; 
				else __moduleResponse = res_not_holding;
			} 
			if(realCase == actual_broke_the_object)
			 __moduleResponse = res_broke_the_object;

			//layer name: '__reward'
			__reward = -100;
			//if (realCase == actual_not_holding) __reward = -10; if(realCase ==  actual_broke_the_object) __reward = -80; if(realCase == actual_pick_action_success) __reward = 100;
		}

		if (ActionManager::actions[actionId]->actionType == placeAction)
		{
			//layer name: 'realCase'
			place_enumRealCase realCase = (place_enumRealCase)Icaps::place_discrete_dist1(Icaps::generator);
			
			//layer name: 'hand_empty'
			state__.handEmpty = (realCase == success || realCase == droppedObject) ? true : false;

			//layer name: 'cupDiscreteGeneralLocation'
			if(realCase == success) state__.cupDiscreteGeneralLocation = state.robotGenerallocation; if(realCase == droppedObject) state__.cupDiscreteGeneralLocation = eUnknown;

			//layer name: '__moduleResponse'
			if (realCase == success) {if (AOSUtils::Bernoulli(0.9)) __moduleResponse = ePlaceActionSuccess; else __moduleResponse = eFailedUnknown;} if (realCase == droppedObject) __moduleResponse = (AOSUtils::Bernoulli(0.9)) ? eDroppedObject : eFailedUnknown; if (realCase == unknownFailure) __moduleResponse = eFailedUnknown;
		
			//layer name: '__reward'
			__reward = -100;
			//__reward = (realCase == success) ? -10 : (realCase == droppedObject) ? -80: -10;
		}

		if (ActionManager::actions[actionId]->actionType == navigateAction)
		{

			NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
			act.SetActionParametersByState(&state);
			tLocation &oDesiredLocation = act.oDesiredLocation;

			//layer name: 'realCase'
			navigate_enumRealCase realCase;
			
			realCase = (navigate_enumRealCase)Icaps::navigate_discrete_dist1(Icaps::generator);

			//layer name: 'state.robotGenerallocation'
			if (realCase == action_success) state__.robotGenerallocation = oDesiredLocation.discrete_location;

			//layer name: '__moduleResponse'
			if (realCase == action_success && AOSUtils::Bernoulli(0.9)) __moduleResponse = eSuccess; else __moduleResponse = eFailed;

			//layer name: '__reward'
			__reward = -100;
			//__reward = -10;
		}
	
		if (ActionManager::actions[actionId]->actionType == observeAction)
		{
			observe_enumRealCase realCase;
			//layer name: 'realCase'
			if (state.robotGenerallocation == state.cupDiscreteGeneralLocation){ if(AOSUtils::Bernoulli(0.8)) realCase = observed; else realCase = notObserved;} else realCase = notObserved;
			
			//layer name: 'state.cupAccurateLocation'
			if (realCase == observed) 
			{ if(AOSUtils::Bernoulli(0.7)) {state__.cupAccurateLocation = true;__moduleResponse = eSuccess;} else{ state__.cupAccurateLocation = false; __moduleResponse = eFailed;}}
			else
			{ if(AOSUtils::Bernoulli(0.3)) {state__.cupAccurateLocation = true;__moduleResponse = eSuccess;} else{ state__.cupAccurateLocation = false; __moduleResponse = eFailed;}}
			//layer name: '__reward'
			__reward = -100;
			//__reward = -10;
		}
		if(startObs == __moduleResponse)
		{
			stringstream ss;
			ss << "Observation/__moduleResponse Not initialized!!! on action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
			loge << ss.str() << endl;
			throw 1;
		}
	}

bool Icaps::ProcessSpecialStates(const IcapsState &state, double &reward) const
{
	bool isFinalState = false;
	if (state.robotGenerallocation == eNear_elevator1 && state.cupDiscreteGeneralLocation == eAuditorium)
	{
		reward += 5000;
		isFinalState = true;
	}
	return isFinalState;
}

//p is between 0 to 1
bool AOSUtils::Bernoulli(double p)
{
	/* generate secret number between 1 and 100: */
	int randInt = rand() % 100 + 1;
	return (p * 100) >= randInt;
}
void ActionDescription::SetActionParametersByState(const IcapsState *state){}
    std::vector<ActionDescription*> ActionManager::actions;

    void NavigateActionDescription::Initialize(NavigateActionDescription* action, int _oDesiredLocation_Index)
    {
		action->actionType = navigateAction;
		action->oDesiredLocation_Index = _oDesiredLocation_Index;
	}

void NavigateActionDescription::SetActionParametersByState(const IcapsState *state)
{
    oDesiredLocation = state->tLocationObjects[oDesiredLocation_Index];
} 

ActionManager::ActionManager(IcapsState* state)
{
	
	int id = 0;
	ActionDescription *pick = new ActionDescription;
	pick->actionType = pickAction;
	pick->actionId = id++;
	ActionManager::actions.push_back(pick);

	ActionDescription *place = new ActionDescription;
    place->actionType = placeAction;
	place->actionId = id++;
    ActionManager::actions.push_back(place);

	ActionDescription *observe = new ActionDescription;
    observe->actionType = observeAction;
	observe->actionId = id++;
    ActionManager::actions.push_back(observe);
	
	NavigateActionDescription* navActions = new NavigateActionDescription[4];
	for (int i = 0; i < 4; i++)
	{
        NavigateActionDescription& navAction = navActions[i];
		NavigateActionDescription::Initialize(&navAction, i);
		navAction.actionId = id++;
		ActionManager::actions.push_back(&navAction);
	}

	for(int j=0;j< ActionManager::actions.size();j++)
	{
		ActionManager::actions[j]->SetActionParametersByState(state);
		logd << "actionID:" << ActionManager::actions[j]->actionId << ", type:" << ActionManager::actions[j]->actionType << ", index:" << ((j < 3) ? 1 : (*(static_cast<NavigateActionDescription *>(ActionManager::actions[j]))).oDesiredLocation_Index);
	}
}

std::string Icaps::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(action, obs);
}

std::string Icaps::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
