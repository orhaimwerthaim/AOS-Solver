
#include "globals.h"
#include "state_var_types.h"
#include <despot/core/pomdp.h>
#include <despot/solver/pomcp.h> 
#include <random>
#include <string>

namespace despot {
     enum ActionType
{
    pickAction,
    placeAction,
	observeAction,
	navigateAction
	
};
/* ==============================================================================
 * IcapsState class
 * ==============================================================================*/
class AOSUtils
{
	public:
	static bool Bernoulli(double);
};

class ActionDescription;
class IcapsState;

class Prints
{
	public:
	static std::string PrintLocation(tDiscreteLocation);
	static std::string PrintActionDescription(ActionDescription*);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(IcapsState state);
	static std::string PrintObs(int action, int obs);
};

class IcapsState : public State
{
public:
	std::vector<tDiscreteLocation> tDiscreteLocationObjects;
	std::vector<tLocation> tLocationObjects;

	tDiscreteLocation cupDiscreteGeneralLocation;
	tDiscreteLocation robotGenerallocation;

	tLocation locationOutside_lab211;
	tLocation locationAuditorium;
	tLocation locationNear_elevator1;
	tLocation locationCorridor;

	anyValue cupAccurateLocation;

	bool handEmpty;
};

/* ==============================================================================
 * Icaps and PocmanBelief class
 * ==============================================================================*/
class Icaps;
class IcapsBelief: public ParticleBelief {
protected:
	const Icaps* icaps_;
public:
	static int num_particles;

	IcapsBelief(std::vector<State*> particles, const DSPOMDP* model, Belief* prior =
		NULL);
	void Update(int actionId, OBS_TYPE obs);
};

/* ==============================================================================
 * Icaps 
 * ==============================================================================*/
/**
 * The implementation is adapted from that included in the POMCP software.
 */

class Icaps: public DSPOMDP {
public:
	virtual std::string PrintObs(int action, OBS_TYPE obs) const;
	virtual std::string PrintStateStr(const State &state) const;
	virtual std::string GetActionDescription(int) const;
	virtual bool Step(State& state, double rand_num, int actionId, double& reward,
		OBS_TYPE& observation) const;
	int NumActions() const;
	virtual double ObsProb(OBS_TYPE obs, const State& state, int actionId) const;

	virtual State* CreateStartState(std::string type = "DEFAULT") const;
	virtual Belief* InitialBelief(const State* start,
		std::string type = "PARTICLE") const;

	inline double GetMaxReward() const {
		return globals::MAX_IMMEDIATE_REWARD;
	}
	 

	inline ValuedAction GetMinRewardAction() const {
		return ValuedAction(0, globals::MIN_IMMEDIATE_REWARD);
	}
	 
	POMCPPrior* CreatePOMCPPrior(std::string name = "DEFAULT") const;

	virtual void PrintState(const State& state, std::ostream& out = std::cout) const;
	

	
	virtual void PrintObs(const State& state, OBS_TYPE observation,
		std::ostream& out = std::cout) const;
	void PrintBelief(const Belief& belief, std::ostream& out = std::cout) const;
	virtual void PrintAction(int actionId, std::ostream& out = std::cout) const;

	State* Allocate(int state_id, double weight) const;
	virtual State* Copy(const State* particle) const;
	virtual void Free(State* particle) const;
	int NumActiveParticles() const;


public:
	Icaps(); 

private:
	void CheckPreconditions(const IcapsState& farstate, double &reward, bool &meetPrecondition, int actionId) const;
	void SampleModuleExecutionTime(const IcapsState& farstate, double rand_num, int actionId, int &moduleExecutionTime) const;
	void ExtrinsicChangesDynamicModel(const IcapsState& initState, IcapsState& afterExState, double rand_num, int actionId, double& reward,
		const int &moduleExecutionTime) const;
	void ModuleDynamicModel(const IcapsState &initState, const IcapsState &afterExState, IcapsState &nextState, double rand_num, int actionId, double &reward,
								 OBS_TYPE &observation, const int &moduleExecutionTime) const;
	bool ProcessSpecialStates(const IcapsState &state, double &reward) const;

	mutable MemoryPool<IcapsState> memory_pool_;
	static std::default_random_engine generator;
	static std::discrete_distribution<> discrete_dist1;
	static std::normal_distribution<double> normal_dist1;
	static std::discrete_distribution<> pick_discrete_dist1;
	static std::normal_distribution<double> place_normal_dist1;
	static std::discrete_distribution<> place_discrete_dist1;
	static std::discrete_distribution<> navigate_discrete_dist1;
};
 
	class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(const IcapsState *state);
};

class NavigateActionDescription: public ActionDescription
{
    public: 
        tLocation oDesiredLocation;
		static void Initialize(NavigateActionDescription *action, int _oDesiredLocation_Index);
		//NavigateActionDescription(int _oDesiredLocation_Index);
		virtual void SetActionParametersByState(const IcapsState *state);
    //private:
        int oDesiredLocation_Index;
};

class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    ActionManager(IcapsState* state);
};

} // namespace despot
 