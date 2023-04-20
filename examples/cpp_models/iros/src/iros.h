
#ifndef IROS_H
#define IROS_H
#include "globals.h"
#include <despot/core/pomdp.h>
#include <despot/solver/pomcp.h> 
#include <random>
#include <string>
#include <despot/model_primitives/iros/enum_map_iros.h>  
#include "closed_model.h"
namespace despot {

/* ==============================================================================
 * IrosState class
 * ==============================================================================*/

class AOSUtils
{
	public:
	static bool Bernoulli(double);
    static int SampleDiscrete(vector<float>);
    static int SampleDiscrete(vector<double>);
	static std::default_random_engine generator;
    static std::uniform_real_distribution<float> real_unfirom_dist;
};
 




/* ==============================================================================
 * Iros and PocmanBelief class
 * ==============================================================================*/
class Iros;
class IrosBelief: public ParticleBelief {
	
public:
    const Iros* iros_;
	static std::string beliefFromDB;
	static int currentInitParticleIndex;
	static int num_particles; 
	IrosBelief(std::vector<State*> particles, const DSPOMDP* model, Belief* prior =
		NULL);
	//void Update(int actionId, OBS_TYPE obs);
	void Update(int actionId, OBS_TYPE obs, std::map<std::string,std::string> localVariablesFromAction);
};

/* ==============================================================================
 * Iros 
 * ==============================================================================*/
/**
 * The implementation is adapted from that included in the POMCP software.
 */

class Iros: public DSPOMDP {
public:
    static Iros gen_model;
    static std::hash<std::string> hasher;
	virtual std::string PrintObs(int action, OBS_TYPE obs) const;
	virtual std::string PrintStateStr(const State &state) const;
	virtual std::string GetActionDescription(int) const;
	void UpdateStateByRealModuleObservation(State &state, int actionId, OBS_TYPE &observation) const;
	virtual bool Step(State &state, double rand_num, int actionId, double &reward,
					  OBS_TYPE &observation) const;
    void StepForModel(State& state, int actionId, double &reward,
                                    OBS_TYPE &observation, int& state_hash, int& next_state_hash, bool &isTerminal, double& precondition_reward, double& specialStateReward) const;
	int NumActions() const;
	virtual double ObsProb(OBS_TYPE obs, const State& state, int actionId) const;
    void CreateAndSolveModel() const;
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
 	static void CheckPreconditions(const IrosState& state, double &reward, bool &meetPrecondition, int actionId);
    static void ComputePreferredActionValue(const IrosState& state, double &__heuristicValue, int actionId);
     
 
	Iros(); 


private:
	void SampleModuleExecutionTime(const IrosState& state, double rand_num, int actionId, int &moduleExecutionTime) const;
	void ExtrinsicChangesDynamicModel(const IrosState& initState, IrosState& afterExState, double rand_num, int actionId,
		const int &moduleExecutionTime,  double &reward) const;
	void ModuleDynamicModel(const IrosState &initState, const IrosState &afterExState, IrosState &nextState, double rand_num, int actionId, double &reward,
								 OBS_TYPE &observation, const int &moduleExecutionTime, const bool &__meetPrecondition) const;
	bool ProcessSpecialStates(IrosState &state, double &reward) const;

	mutable MemoryPool<IrosState> memory_pool_;
	static std::default_random_engine generator;

};
} // namespace despot
#endif//IROS_H
 