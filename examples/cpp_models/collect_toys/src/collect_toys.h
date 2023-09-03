
#ifndef COLLECT_TOYS_H
#define COLLECT_TOYS_H
#include "globals.h"
#include <despot/core/pomdp.h>
#include <despot/solver/pomcp.h> 
#include <random>
#include <string>
#include <despot/model_primitives/collect_toys/enum_map_collect_toys.h>  


namespace despot {

/* ==============================================================================
 * Collect_toysState class
 * ==============================================================================*/



class AOSUtils
{
	public:
	static bool Bernoulli(double);
    static int SampleDiscrete(vector<float>);
    static int SampleDiscrete(vector<double>);
	static std::default_random_engine generator;
    static std::uniform_real_distribution<float> real_unfirom_dist;
	static int get_hash(string str);
};
 




/* ==============================================================================
 * Collect_toys and PocmanBelief class
 * ==============================================================================*/
class Collect_toys;
class Collect_toysBelief: public ParticleBelief {
	
public:
    const Collect_toys* collect_toys_;
	static std::string beliefFromDB;
	static int currentInitParticleIndex;
	static int num_particles; 
	Collect_toysBelief(std::vector<State*> particles, const DSPOMDP* model, Belief* prior =
		NULL);
	//void Update(int actionId, OBS_TYPE obs);
	void Update(int actionId, OBS_TYPE obs, std::map<std::string,std::string> localVariablesFromAction);
};

/* ==============================================================================
 * Collect_toys 
 * ==============================================================================*/
/**
 * The implementation is adapted from that included in the POMCP software.
 */

class Collect_toys: public DSPOMDP {
public:
    static Collect_toys gen_model;
    virtual std::string PrintObs(int action, OBS_TYPE obs) const;
	virtual std::string PrintStateStr(const State &state) const;
	virtual std::string GetActionDescription(int) const;
	void UpdateStateByRealModuleObservation(State &state, int actionId, OBS_TYPE &observation) const;
	virtual bool Step(State &state, double rand_num, int actionId, double &reward,
					  OBS_TYPE &observation) const;
    void StepForModel(State& state, int actionId, double &reward,
                                    OBS_TYPE &observation, int& state_hash, int& next_state_hash, bool &isTerminal, double& precondition_reward, double& specialStateReward) const;
	int GetStateHash(State state) const;
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
 	static void CheckPreconditions(const Collect_toysState& state, double &reward, bool &meetPrecondition, int actionId);
    static void ComputePreferredActionValue(const Collect_toysState& state, double &__heuristicValue, int actionId);
     
 
	Collect_toys(); 


private:
	void SampleModuleExecutionTime(const Collect_toysState& state, double rand_num, int actionId, int &moduleExecutionTime) const;
	void ExtrinsicChangesDynamicModel(const Collect_toysState& initState, Collect_toysState& afterExState, double rand_num, int actionId,
		const int &moduleExecutionTime,  double &reward) const;
	void ModuleDynamicModel(const Collect_toysState &initState, const Collect_toysState &afterExState, Collect_toysState &nextState, double rand_num, int actionId, double &reward,
								 OBS_TYPE &observation, const int &moduleExecutionTime, const bool &__meetPrecondition) const;
	bool ProcessSpecialStates(Collect_toysState &state, double &reward) const;

	mutable MemoryPool<Collect_toysState> memory_pool_;
	static std::default_random_engine generator;

};
} // namespace despot
#endif//COLLECT_TOYS_H
 