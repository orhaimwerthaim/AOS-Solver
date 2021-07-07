#include "icaps.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/icaps/actionManager.h> 
#include <despot/model_primitives/icaps/enum_map_icaps.h> 
#include <despot/model_primitives/icaps/state.h> 

using namespace std;

namespace despot {



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
	IcapsResponseModuleAndTempEnums eObs = (IcapsResponseModuleAndTempEnums)obs;
	switch (eObs)
	{ 
		case pick_res_pick_action_success:
			return "pick_res_pick_action_success";
		case pick_res_not_holding:
			return "pick_res_not_holding";
		case pick_res_broke_the_object:
			return "pick_res_broke_the_object";  
	 
		case place_ePlaceActionSuccess:
			return "ePlaceActionSuccess";
		case place_eDroppedObject:
			return "place_eDroppedObject";
		case place_eFailedUnknown:
			return "place_eFailedUnknown";  
		  
		case navigate_eSuccess:
			return "navigate_eSuccess";
		case navigate_eFailed:
			return "navigate_eFailed";  
		  
		case observe_eObserved:
			return "observe_eObserved";
		case observe_eNotObserved:
			return "observe_eNotObserved";  
		case illegalActionObs:
			return "illegalActionObs";
		default:
			return "Error obs(unknown action observation)!!!";
		
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


void IcapsBelief::Update(int actionId, OBS_TYPE obs, std::map<std::string,bool> updates) {
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
				IcapsState &icaps_particle = static_cast<IcapsState &>(*particle);
				if(!Globals::IsInternalSimulation() && updates.size() > 0)
				{
					IcapsState::SetAnyValueLinks(&icaps_particle);
					map<std::string, bool>::iterator it;
					for (it = updates.begin(); it != updates.end(); it++)
					{
						*(icaps_particle.anyValueUpdateDic[it->first]) = it->second; 
					} 
				}
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

	startState->tLocationObjectsForActions["state.locationOutside_lab211"] = (state.locationOutside_lab211);
	startState->tLocationObjectsForActions["state.locationAuditorium"]=(state.locationAuditorium);
	startState->tLocationObjectsForActions["state.locationNear_elevator1"]=(state.locationNear_elevator1);
	startState->tLocationObjectsForActions["state.locationCorridor"]=(state.locationCorridor);

	state.robotGenerallocation = state.locationNear_elevator1.discrete_location;
	state.cupAccurateLocation = false;
	//generated from environment file line: state.cupDiscreteGeneralLocation = AOS.SampleDiscrete(tDiscreteLocation,{0.6, 0.4,0,0});
	state.cupDiscreteGeneralLocation = state.tDiscreteLocationObjects[discrete_dist1(generator)];

 
	if (ActionManager::actions.size() == 0)
	{
		ActionManager::Init(const_cast <IcapsState*> (startState));
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
		ActionType &actType = ActionManager::actions[actionId]->actionType;
		meetPrecondition = false;
		//Pick PLP Line: "global_variable_conditionCode": "AOS.IsInitialized(state.cupAccurateLocation) && hand_empty == true && state.robotGenerallocation == state.cupDiscreteGeneralLocation",
		if(actType == pickAction)
		{
			if(state.cupAccurateLocation == true && state.handEmpty == true && state.robotGenerallocation == state.cupDiscreteGeneralLocation)
			{
				meetPrecondition = true;
				 
			}
			 
		}

		//Place PLP Line: "dynamicModelPreconditions": "state.handEmpty == false && (state.robotGenerallocatio == eAuditorium || state.robotGenerallocatio == eOutside_lab211 || state.robotGenerallocatio == eCorridor)",
		if(actType == placeAction)
		{
			if(state.handEmpty == false && (state.robotGenerallocation == eAuditorium || state.robotGenerallocation == eOutside_lab211 || state.robotGenerallocation == eCorridor))
			{
				meetPrecondition = true;
				 
			} 
		}

		if(actType == navigateAction)
		{ 
			meetPrecondition = true;
			 
		}

		if(actType == observeAction)
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
		ActionType &actType = ActionManager::actions[actionId]->actionType;
		if(actType == pickAction)
		{
			__moduleExecutionTime = Icaps::normal_dist1(Icaps::generator);
		}
		if(actType == placeAction)
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
		ActionType &actType = ActionManager::actions[actionId]->actionType;
		observation = -1;
		int startObs = observation;
		OBS_TYPE &__moduleResponse = observation;

		if (actType == pickAction)
		{
			//layer name: 'realCase'
			IcapsResponseModuleAndTempEnums realCase = (IcapsResponseModuleAndTempEnums)(pick_enumRealCase + 1 + Icaps::pick_discrete_dist1(Icaps::generator));
			
			//layer name: 'handEmpty'
			//TODO: uncomment and delete the next line
			state__.handEmpty = (realCase == pick_actual_pick_action_success) ? false: true;
			
			//layer name: 'cupDiscreteGeneralLocation'
			if(realCase == pick_actual_pick_action_success || realCase == pick_actual_broke_the_object)
			{
				state__.cupDiscreteGeneralLocation = eUnknown; state__.cupAccurateLocation = false;
			}
			
			//layer name: '__moduleResponse'
			if (realCase == pick_actual_not_holding) 
			{
				if (AOSUtils::Bernoulli(0.9))
					__moduleResponse= pick_res_not_holding; 
				else __moduleResponse = pick_res_pick_action_success;
			}
			if (realCase == pick_actual_pick_action_success) 
			{
				if (AOSUtils::Bernoulli(0.9)) 
					__moduleResponse = pick_res_pick_action_success; 
				else __moduleResponse = pick_res_not_holding;
			} 
			if(realCase == pick_actual_broke_the_object)
			 __moduleResponse = pick_res_broke_the_object;

			//layer name: '__reward'
			__reward = -100;
			//if (realCase == pick_actual_not_holding) __reward = -10; if(realCase ==  pick_actual_broke_the_object) __reward = -80; if(realCase == pick_actual_pick_action_success) __reward = 100;
		}

		if (actType == placeAction)
		{
			//layer name: 'realCase'
			IcapsResponseModuleAndTempEnums realCase = (IcapsResponseModuleAndTempEnums)(place_enumRealCase + 1 + Icaps::place_discrete_dist1(Icaps::generator));
			
			//layer name: 'hand_empty'
			state__.handEmpty = (realCase == place_success || realCase == place_droppedObject) ? true : false;

			//layer name: 'cupDiscreteGeneralLocation'
			if(realCase == place_success) state__.cupDiscreteGeneralLocation = state.robotGenerallocation; if(realCase == place_droppedObject) state__.cupDiscreteGeneralLocation = eUnknown;

			//layer name: '__moduleResponse'
			if (realCase == place_success) {if (AOSUtils::Bernoulli(0.9)) __moduleResponse = place_ePlaceActionSuccess; else __moduleResponse = place_eFailedUnknown;} if (realCase == place_droppedObject) __moduleResponse = (AOSUtils::Bernoulli(0.9)) ? place_eDroppedObject : place_eFailedUnknown; if (realCase == place_unknownFailure) __moduleResponse = place_eFailedUnknown;
		
			//layer name: '__reward'
			__reward = -100;
			//__reward = (realCase == place_success) ? -10 : (realCase == place_droppedObject) ? -80: -10;
		}

		if (actType == navigateAction)
		{

			NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
			
			tLocation &oDesiredLocation = act.oDesiredLocation;

			//layer name: 'realCase'
			IcapsResponseModuleAndTempEnums realCase;
			
			realCase = (IcapsResponseModuleAndTempEnums)(navigate_enumRealCase + 1 + Icaps::navigate_discrete_dist1(Icaps::generator));

			//layer name: 'state.robotGenerallocation'
			if (realCase == navigate_action_success) state__.robotGenerallocation = oDesiredLocation.discrete_location;

			//layer name: '__moduleResponse'
			if (realCase == navigate_action_success && AOSUtils::Bernoulli(0.9)) __moduleResponse = navigate_eSuccess; else __moduleResponse = navigate_eFailed;

			//layer name: '__reward'
			__reward = -100;
			//__reward = -10;
		}
	
		if (actType == observeAction)
		{
			IcapsResponseModuleAndTempEnums realCase;
			//layer name: 'realCase'
			if (state.robotGenerallocation == state.cupDiscreteGeneralLocation){ if(AOSUtils::Bernoulli(0.8)) realCase = observe_observed; else realCase = observe_notObserved;} else realCase = observe_notObserved;
			
			//layer name: 'state.cupAccurateLocation'
			if (realCase == observe_observed) 
			{ if(AOSUtils::Bernoulli(0.7)) {state__.cupAccurateLocation = true;__moduleResponse = observe_eObserved;} else{ state__.cupAccurateLocation = false; __moduleResponse = observe_eNotObserved;}}
			else
			{ if(AOSUtils::Bernoulli(0.3)) {state__.cupAccurateLocation = true;__moduleResponse = observe_eObserved;} else{ state__.cupAccurateLocation = false; __moduleResponse = observe_eNotObserved;}}
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
/*void ActionDescription::SetActionParametersByState(const IcapsState *state){}
  //  std::vector<ActionDescription*> ActionManager::actions;

    void NavigateActionDescription::Initialize(NavigateActionDescription* action, int _oDesiredLocation_Index)
    {
		action->actionType = navigateAction;
		action->oDesiredLocation_Index = _oDesiredLocation_Index;
	}

void NavigateActionDescription::SetActionParametersByState(const IcapsState *state)
{
    oDesiredLocation = state->tLocationObjectsForActions[oDesiredLocation_Index];
} 


*/
std::string Icaps::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(action, obs);
}

std::string Icaps::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
