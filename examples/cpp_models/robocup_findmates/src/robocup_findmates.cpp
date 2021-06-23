#include "robocup_findmates.h"
#include <despot/core/pomdp.h> 

#include <despot/solver/pomcp.h>


using namespace std;

namespace despot {

/* ==============================================================================
 *FindMatesBelief class
 * ==============================================================================*/
int FindMatesBelief::num_particles = 50000;


FindMatesBelief::FindMatesBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	findmates_(static_cast<const FindMates*>(model)) {
}

void FindMatesBelief::Update(int action, OBS_TYPE obs) {
	history_.Add(action, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = findmates_->Copy(particles_[cur]);
		bool terminal = findmates_->Step(*particle, Random::RANDOM.NextDouble(),
			action, reward, o);

		if (!terminal && o == obs)
			//|| findmates_->LocalMove(*particle, history_, obs)) 
			{
			updated.push_back(particle);
		} else {
			findmates_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		findmates_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
}

/* ==============================================================================
 * FindMates class
 * ==============================================================================*/

FindMates::FindMates(){
	// NumObservations = 1 << 10;
	// See ghost N
	// See ghost E
	// See ghost S
	// See ghost W
	// Can move N
	// Can move E
	// Can move S
	// Can move W
	// Smell food
	// Hear ghost
}






int FindMates::NumActions() const {
	return globals::NUM_OF_ACTIONS;
}

double FindMates::ObsProb(OBS_TYPE obs, const State& state, int action) const {
	return 0.9;
}

 


std::default_random_engine FindMates::generator;

//generated for line: sampleDiscrete(tName,{0.25, 0.25,0.25,0.25});
std::discrete_distribution<> FindMates::dist1{0.25, 0.25,0.25,0.25};


State* FindMates::CreateStartState(string tyep) const {
	FindMatesState* startState = memory_pool_.Allocate();
	startState->person1 = tPerson();
	startState->person2 = tPerson();
	startState->person3 = tPerson();
	startState->person4 = tPerson();

	startState->tPersonObjects.push_back(startState->person1);
	startState->tPersonObjects.push_back(startState->person2);
	startState->tPersonObjects.push_back(startState->person3);
	startState->tPersonObjects.push_back(startState->person4);

	tName name1 = tName();
	tName name2 = tName();
	tName name3 = tName();
	tName name4 = tName();

	startState->tNameObjects.push_back(startState->name1);
	startState->tNameObjects.push_back(startState->name2);
	startState->tNameObjects.push_back(startState->name3);
	startState->tNameObjects.push_back(startState->name4);

	startState->reported = false;
	
	startState->name1.enumName = eName1; startState->name1.actual_name = "Ronen";
	startState->name2.enumName = eName2; startState->name2.actual_name = "Tal";
	startState->name3.enumName = eName3; startState->name3.actual_name = "Or";
	startState->name4.enumName = eName4; startState->name4.actual_name = "Shashank";
	for (int i = 0; i < startState->tPersonObjects.size();i++)//generated for line: "forEach":[{"globalArrayByType":"tPerson", "itemNameInCode":"_person"}],
	{
		tPerson &_person = startState->tPersonObjects[i];
		_person.nameConfirmed = false;_person.reported=false;
		_person.name = &startState->tNameObjects[dist1(generator)];//generated for line: sampleDiscrete(tName,{0.25, 0.25,0.25,0.25});
	}
 
	return startState;
}



Belief* FindMates::InitialBelief(const State* start, string type) const {
	int N = FindMatesBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}

	return new FindMatesBelief(particles, this);
}
 

 

POMCPPrior* FindMates::CreatePOMCPPrior(string name) const { 
		return new UniformPOMCPPrior(this);
}

void FindMates::PrintState(const State& state, ostream& ostr) const {
	const FindMatesState& farstate = static_cast<const FindMatesState&>(state);
	ostr << "called PrintState(): state printed"<<endl;
	
}

void FindMates::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const FindMatesState& farstate = static_cast<const FindMatesState&>(state);
	
	ostr << "called PrintObs(): obs printed"<<endl;
}

void FindMates::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void FindMates::PrintAction(int action, ostream& out) const {
	out << "action:" << action << endl;
}

State* FindMates::Allocate(int state_id, double weight) const {
	FindMatesState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* FindMates::Copy(const State* particle) const {
	FindMatesState* state = memory_pool_.Allocate();
	*state = *static_cast<const FindMatesState*>(particle);
	state->SetAllocated();
	return state;
}

void FindMates::Free(State* particle) const {
	memory_pool_.Free(static_cast<FindMatesState*>(particle));
}

int FindMates::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

bool FindMates::Step(State& state, double rand_num, int action, double& reward,
	OBS_TYPE& observation) const {
	bool isNextStateFinal = false;
	Random random(rand_num);
	int moduleExecutionTime = -1;
	bool meetPrecondition = false;

	FindMatesState& farstate = static_cast<FindMatesState&>(state);
	CheckPreconditions(farstate, reward, meetPrecondition);
	if (!meetPrecondition)
	{
		return false;
	}

	State *s_initState = Copy(&state);
	FindMatesState &initState = static_cast<FindMatesState &>(*s_initState);

	
	SampleModuleExecutionTime(farstate, rand_num, action, moduleExecutionTime);

	ExtrinsicChangesDynamicModel(initState, farstate, rand_num, action, reward, moduleExecutionTime);

	State *s_afterExState = Copy(&state);
	FindMatesState &afterExState = static_cast<FindMatesState &>(*s_afterExState);

	ModuleDynamicModel(initState, afterExState, farstate, rand_num, action, reward,
					   observation, moduleExecutionTime);
	//FindMatesState& afterExtrinsicChanges = static_cast<FindMatesState&>(Copy(&state));
	
	Free(s_initState);
	Free(s_afterExState);
	return CheckIfFinalState(farstate);
}

void FindMates::CheckPreconditions(const FindMatesState& farstate, double &reward, bool &meetPrecondition) const
	{
	}

void FindMates::SampleModuleExecutionTime(const FindMatesState& farstate, double rand_num, int action, int &moduleExecutionTime) const
	{
		int i = 2;
	}

void FindMates::ExtrinsicChangesDynamicModel(const FindMatesState& initState, FindMatesState& afterExState, double rand_num, int action, double& reward,
		const int &moduleExecutionTime)  const
	{	
		int i = 2;
	}

	void FindMates::ModuleDynamicModel(const FindMatesState &initState, const FindMatesState &afterExState, FindMatesState &nextState, double rand_num, int action, double &reward,
									  OBS_TYPE &observation, const int &moduleExecutionTime) const
	{
		int i = 2;
	}

bool FindMates::CheckIfFinalState(const FindMatesState &farstate) const
{
	return false;
}
}// namespace despot
