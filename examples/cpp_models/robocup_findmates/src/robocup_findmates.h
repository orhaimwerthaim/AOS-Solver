
#include "globals.h"
#include "state_var_types.h"
#include <despot/core/pomdp.h>
#include <despot/solver/pomcp.h> 
#include <random>
#include <string>

namespace despot {

/* ==============================================================================
 * FindMatesState class
 * ==============================================================================*/
class FindMatesState: public State {
public:
	std::vector<tPerson> tPersonObjects;
	std::vector<tName> tNameObjects;
	tPerson person1;
	tPerson person2;
	tPerson person3;
	tPerson person4;
	bool reported;
	tName name1;
	tName name2;
	tName name3;
	tName name4;
};

 

/* ==============================================================================
 * FindMates and PocmanBelief class
 * ==============================================================================*/
class FindMates;
class FindMatesBelief: public ParticleBelief {
protected:
	const FindMates* findmates_;
public:
	static int num_particles;

	FindMatesBelief(std::vector<State*> particles, const DSPOMDP* model, Belief* prior =
		NULL);
	void Update(int action, OBS_TYPE obs);
};

/* ==============================================================================
 * FindMates 
 * ==============================================================================*/
/**
 * The implementation is adapted from that included in the POMCP software.
 */
class FindMates: public DSPOMDP {
public:
	virtual bool Step(State& state, double rand_num, int action, double& reward,
		OBS_TYPE& observation) const;
	int NumActions() const;
	virtual double ObsProb(OBS_TYPE obs, const State& state, int action) const;

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
	virtual void PrintAction(int action, std::ostream& out = std::cout) const;

	State* Allocate(int state_id, double weight) const;
	virtual State* Copy(const State* particle) const;
	virtual void Free(State* particle) const;
	int NumActiveParticles() const;


public:
	FindMates(); 

private:
	void CheckPreconditions(const FindMatesState& farstate, double &reward, bool &meetPrecondition) const;
	void SampleModuleExecutionTime(const FindMatesState& farstate, double rand_num, int action, int &moduleExecutionTime) const;
	void ExtrinsicChangesDynamicModel(const FindMatesState& initState, FindMatesState& afterExState, double rand_num, int action, double& reward,
		const int &moduleExecutionTime) const;
	void ModuleDynamicModel(const FindMatesState &initState, const FindMatesState &afterExState, FindMatesState &nextState, double rand_num, int action, double &reward,
								 OBS_TYPE &observation, const int &moduleExecutionTime) const;
	bool CheckIfFinalState(const FindMatesState &farstate) const;

	mutable MemoryPool<FindMatesState> memory_pool_;
	static std::default_random_engine generator;
	static std::discrete_distribution<> dist1;
};

  

} // namespace despot
 