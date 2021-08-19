#include "icaps2.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/icaps2/actionManager.h> 
#include <despot/model_primitives/icaps2/enum_map_icaps2.h> 
#include <despot/model_primitives/icaps2/state.h> 

using namespace std;

namespace despot {


bool AOSUtils::Bernoulli(double p)
{
	/* generate secret number between 1 and 100: */
	int randInt = rand() % 100 + 1;
	return (p * 100) >= randInt;
}

/* ==============================================================================
 *Icaps2Belief class
 * ==============================================================================*/
int Icaps2Belief::num_particles = 5000;


Icaps2Belief::Icaps2Belief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	icaps2_(static_cast<const Icaps2*>(model)) {
}
 
    std::string Prints::PrinttDiscreteLocation(tDiscreteLocation enumT)
    {
        switch (enumT)
        {
            case eOutside_lab211:
                return "eOutside_lab211";
            case eAuditorium:
                return "eAuditorium";
            case eCorridor:
                return "eCorridor";
            case eNear_elevator1:
                return "eNear_elevator1";
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
            NavigateActionDescription *navigateA = static_cast<NavigateActionDescription *>(act);
            ss << "," << "discrete_location:" << Prints::PrinttDiscreteLocation((tDiscreteLocation)navigateA->oDesiredLocation.discrete_location);;
            ss << "," << "actual_location:" << navigateA->oDesiredLocation.actual_location;
        }

        return ss.str();
    }


std::string Prints::PrintObs(int action, int obs)
{
	Icaps2ResponseModuleAndTempEnums eObs = (Icaps2ResponseModuleAndTempEnums)obs;
	return enum_map_icaps2::vecResponseEnumToString[eObs]; 
}
    std::string Prints::PrintState(Icaps2State state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|cupDiscreteGeneralLocation:";
        ss <<  Prints::PrinttDiscreteLocation(state.cupDiscreteGeneralLocation);
        ss << "|cupAccurateLocation:";
        ss <<  state.cupAccurateLocation;
        ss << "|handEmpty:";
        ss <<  state.handEmpty;
        ss << "|robotGenerallocation:";
        ss <<  Prints::PrinttDiscreteLocation(state.robotGenerallocation);
        return ss.str();
    }


	
 	std::string Icaps2::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case placeAction:
            return "placeAction";
        case observeAction:
            return "observeAction";
        case pickAction:
            return "pickAction";
        case navigateAction:
            return "navigateAction";
        }
    }



void Icaps2Belief::Update(int actionId, OBS_TYPE obs, std::map<std::string,bool> updates) {
	history_.Add(actionId, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = icaps2_->Copy(particles_[cur]);
		bool terminal = icaps2_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		if (!terminal && o == obs) 
			{
				Icaps2State &icaps2_particle = static_cast<Icaps2State &>(*particle);
				if(!Globals::IsInternalSimulation() && updates.size() > 0)
				{
					Icaps2State::SetAnyValueLinks(&icaps2_particle);
					map<std::string, bool>::iterator it;
					for (it = updates.begin(); it != updates.end(); it++)
					{
						*(icaps2_particle.anyValueUpdateDic[it->first]) = it->second; 
					} 
				}
				updated.push_back(particle);
		} else {
			icaps2_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		icaps2_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
}

/* ==============================================================================
 * Icaps2 class
 * ==============================================================================*/

Icaps2::Icaps2(){
	
}

int Icaps2::NumActions() const {
	return ActionManager::actions.size();
}

double Icaps2::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Icaps2::generator;

std::normal_distribution<double>  Icaps2::place_normal_dist1(30000,15000); //AOS.SampleNormal(30000,15000)
std::normal_distribution<double>  Icaps2::observe_normal_dist2(15000,2000); //AOS.SampleNormal(15000,2000)
std::normal_distribution<double>  Icaps2::pick_normal_dist3(40000,10000); //AOS.SampleNormal(40000,10000)
std::normal_distribution<double>  Icaps2::navigate_normal_dist4(40000,10000); //AOS.SampleNormal(40000,10000)
std::discrete_distribution<> Icaps2::place_discrete_dist1{0.8,0.01,0}; //AOS.SampleDiscrete(enumRealCase,{0.8,0.01,0})
std::discrete_distribution<> Icaps2::pick_discrete_dist2{0.8,0.1,0,0.1}; //AOS.SampleDiscrete(enumRealCase,{0.8,0.1,0,0.1})
std::discrete_distribution<> Icaps2::navigate_discrete_dist3{0.8,0.2}; //AOS.SampleDiscrete(enumRealCase,{0.8,0.2})
std::discrete_distribution<> Icaps2::Environment_discrete_dist4{0.04,0,0.6}; //AOS.SampleDiscrete(tDiscreteLocation,{0.04,0,0.6})

State* Icaps2::CreateStartState(string tyep) const {
    Icaps2State* startState = memory_pool_.Allocate();
    Icaps2State& state = *startState;
    startState->tDiscreteLocationObjects.push_back(eOutside_lab211);
    startState->tDiscreteLocationObjects.push_back(eAuditorium);
    startState->tDiscreteLocationObjects.push_back(eCorridor);
    startState->tDiscreteLocationObjects.push_back(eNear_elevator1);
    startState->tDiscreteLocationObjects.push_back(eUnknown);
    state.cupDiscreteGeneralLocation = eCorridor;
    state.cupAccurateLocation = false;
    state.handEmpty = true;
    state.locationOutside_lab211 = tLocation();
    state.locationOutside_lab211.discrete_location = eOutside_lab211;
     state.locationOutside_lab211.actual_location = true;
    state.locationAuditorium = tLocation();
    state.locationAuditorium.discrete_location = eAuditorium;
     state.locationAuditorium.actual_location = true;
    state.locationNear_elevator1 = tLocation();
    state.locationNear_elevator1.discrete_location = eNear_elevator1;
     state.locationNear_elevator1.actual_location = true;
    state.locationCorridor = tLocation();
    state.locationCorridor.discrete_location = eCorridor;
     state.locationCorridor.actual_location = true;
    startState->tLocationObjectsForActions["state.locationOutside_lab211"] = (state.locationOutside_lab211);
    startState->tLocationObjectsForActions["state.locationAuditorium"] = (state.locationAuditorium);
    startState->tLocationObjectsForActions["state.locationNear_elevator1"] = (state.locationNear_elevator1);
    startState->tLocationObjectsForActions["state.locationCorridor"] = (state.locationCorridor);
    state.robotGenerallocation=state.locationNear_elevator1.discrete_location;
    state.cupDiscreteGeneralLocation=state.tDiscreteLocationObjects[Environment_discrete_dist4(generator)];;
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <Icaps2State*> (startState));
    }
    return startState;
}



Belief* Icaps2::InitialBelief(const State* start, string type) const {
	int N = Icaps2Belief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}

	return new Icaps2Belief(particles, this);
}
 

 

POMCPPrior* Icaps2::CreatePOMCPPrior(string name) const { 
		return new UniformPOMCPPrior(this);
}

void Icaps2::PrintState(const State& state, ostream& ostr) const {
	const Icaps2State& farstate = static_cast<const Icaps2State&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
}

void Icaps2::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const Icaps2State& farstate = static_cast<const Icaps2State&>(state);
	
	ostr << observation <<endl;
}

void Icaps2::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Icaps2::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Icaps2::Allocate(int state_id, double weight) const {
	Icaps2State* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Icaps2::Copy(const State* particle) const {
	Icaps2State* state = memory_pool_.Allocate();
	*state = *static_cast<const Icaps2State*>(particle);
	state->SetAllocated();
	return state;
}

void Icaps2::Free(State* particle) const {
	memory_pool_.Free(static_cast<Icaps2State*>(particle));
}

int Icaps2::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

bool Icaps2::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
	bool isNextStateFinal = false;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	
	Icaps2State &state__ = static_cast<Icaps2State &>(s_state__);
	 logd << "[Icaps2::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	if (!meetPrecondition)
	{
		__moduleExecutionTime = 0;
		observation = illegalActionObs;
		return false;
	}

	State *s_state = Copy(&s_state__);
	Icaps2State &state = static_cast<Icaps2State &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, reward, __moduleExecutionTime);

	State *s_state_ = Copy(&s_state__);
	Icaps2State &state_ = static_cast<Icaps2State &>(*s_state_);

	ModuleDynamicModel(state, state_, state__, rand_num, actionId, reward,
					   observation, __moduleExecutionTime);
	
	Free(s_state);
	Free(s_state_);
	bool finalState = ProcessSpecialStates(state__, reward);
	return finalState;
}

void Icaps2::CheckPreconditions(const Icaps2State& state, double &reward, bool &meetPrecondition, int actionId) const
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        meetPrecondition = false;
            if(actType == placeAction)
            {
                if(state.handEmpty == false && (state.robotGenerallocation == eAuditorium || state.robotGenerallocation == eOutside_lab211 || state.robotGenerallocation == eCorridor) && true)
                {
                    meetPrecondition = true;
                }
                else
                {
                    reward += -800;
                }
            }
            if(actType == observeAction)
            {
                if(true && true)
                {
                    meetPrecondition = true;
                }
                else
                {
                    reward += 0;
                }
            }
            if(actType == pickAction)
            {
                if((state.cupAccurateLocation == true) && state.handEmpty == true && state.robotGenerallocation == state.cupDiscreteGeneralLocation && true)
                {
                    meetPrecondition = true;
                }
                else
                {
                    reward += -800;
                }
            }
            if(actType == navigateAction)
            {
                if(state.robotGenerallocation != eUnknown && true)
                {
                    meetPrecondition = true;
                }
                else
                {
                    reward += -800;
                }
            }
    }

void Icaps2::SampleModuleExecutionTime(const Icaps2State& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == placeAction)
    {
        __moduleExecutionTime=Icaps2::place_normal_dist1(Icaps2::generator);
    }
    if(actType == observeAction)
    {
        __moduleExecutionTime=Icaps2::observe_normal_dist2(Icaps2::generator);
    }
    if(actType == pickAction)
    {
        __moduleExecutionTime=Icaps2::pick_normal_dist3(Icaps2::generator);
    }
    if(actType == navigateAction)
    {
        __moduleExecutionTime=Icaps2::pick_normal_dist3(Icaps2::generator);
    }
}

void Icaps2::ExtrinsicChangesDynamicModel(const Icaps2State& state, Icaps2State& state_, double rand_num, int actionId, double& reward, const int &__moduleExecutionTime)  const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
}

void Icaps2::ModuleDynamicModel(const Icaps2State &state, const Icaps2State &state_, Icaps2State &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    observation = -1;
    int startObs = observation;
    OBS_TYPE &__moduleResponse = observation;
    if(actType == placeAction)
    {
        Icaps2ResponseModuleAndTempEnums  realCase;
        realCase=(Icaps2ResponseModuleAndTempEnums)(place_enumRealCase + 1 + Icaps2::place_discrete_dist1(Icaps2::generator));
        state__.handEmpty=(realCase==place_success||realCase==place_droppedObject)?true:false;
        if(realCase==place_success)state__.cupDiscreteGeneralLocation=state.robotGenerallocation;
        if(realCase==place_droppedObject)state__.cupDiscreteGeneralLocation=eUnknown;
        if(realCase==place_success){if(AOSUtils::Bernoulli(0.9))__moduleResponse=place_ePlaceActionSuccess;
        else __moduleResponse=place_eFailedUnknown;
        }if(realCase==place_droppedObject)__moduleResponse=(AOSUtils::Bernoulli(0.9))?place_eDroppedObject:place_eFailedUnknown;
        if(realCase==place_unknownFailure)__moduleResponse=place_eFailedUnknown;
        __reward=-100;
    }
    if(actType == observeAction)
    {
        Icaps2ResponseModuleAndTempEnums  realCase;
        if(state.robotGenerallocation==state.cupDiscreteGeneralLocation){if(AOSUtils::Bernoulli(0.99))realCase=observe_observed;
        else realCase=observe_notObserved;
        }else realCase=observe_notObserved;
        if(realCase==observe_observed&&AOSUtils::Bernoulli(0.9)){state__.cupAccurateLocation = true;
        __moduleResponse=observe_eObserved;
        }else {state__.cupAccurateLocation = false;
        __moduleResponse=observe_eNotObserved;
        };
        __reward=-100;
    }
    if(actType == pickAction)
    {
        Icaps2ResponseModuleAndTempEnums  realCase;
        realCase=(Icaps2ResponseModuleAndTempEnums)(pick_enumRealCase + 1 + Icaps2::pick_discrete_dist2(Icaps2::generator));
        state__.handEmpty=(realCase==pick_actual_pick_action_success)?false:true;
        if(realCase==pick_actual_pick_action_success||realCase==pick_actual_broke_the_object){state__.cupDiscreteGeneralLocation=eUnknown;
        state__.cupAccurateLocation = false;
        };
        if(realCase==pick_actual_not_holding){if(AOSUtils::Bernoulli(0.9))__moduleResponse=pick_res_not_holding;
        else __moduleResponse=pick_res_pick_action_success;
        }if(realCase==pick_actual_pick_action_success){if(AOSUtils::Bernoulli(0.9))__moduleResponse=pick_res_pick_action_success;
        else __moduleResponse=pick_res_not_holding;
        }else __moduleResponse=pick_res_broke_the_object;
        __reward=-100;
    }
    if(actType == navigateAction)
    {
        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
        tLocation &oDesiredLocation = act.oDesiredLocation;
        Icaps2ResponseModuleAndTempEnums  realCase;
        realCase=(Icaps2ResponseModuleAndTempEnums)(navigate_enumRealCase + 1 + Icaps2::navigate_discrete_dist3(Icaps2::generator));
        if(realCase==navigate_action_success)state__.robotGenerallocation=oDesiredLocation.discrete_location;
        if(realCase==navigate_action_success&&AOSUtils::Bernoulli(0.9))__moduleResponse=navigate_eSuccess;
        else __moduleResponse=navigate_eFailed;
        __reward=-100;
    }
    if(startObs == __moduleResponse)
    {
    stringstream ss;
    ss << "Observation/__moduleResponse Not initialized!!! on action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
    loge << ss.str() << endl;
    throw 1;
    }
}

bool Icaps2::ProcessSpecialStates(const Icaps2State &state, double &reward) const
{
    bool isFinalState = false;
    if (state.robotGenerallocation == eNear_elevator1 && state.cupDiscreteGeneralLocation == eAuditorium)
    {
        reward += 8000;
        isFinalState = true;
    }
    return isFinalState;
}





std::string Icaps2::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(action, obs);
}

std::string Icaps2::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
